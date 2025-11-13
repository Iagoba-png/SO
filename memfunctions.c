/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba  iago.alcoba@udc.es
    GRUPO: 1.3
*/

#include "memfunctions.h"

int Globald = 0, Globale = 1, Globalf = 2, GlobalNd, GlobalNe, GlobalNf;


void liberarMemoria(int numeroBloques, struct bloqab tablaBloques[]) {
    for (int i = 0; i < numeroBloques; i++) {
        if (strcmp(tablaBloques[i].modoMemoria, "malloc") == 0) free(tablaBloques[i].direccionMemoria);
        if (strcmp(tablaBloques[i].modoMemoria, "shared") == 0) shmdt(tablaBloques[i].direccionMemoria);
        if (strcmp(tablaBloques[i].modoMemoria, "mmap") == 0)
            munmap(tablaBloques[i].direccionMemoria, tablaBloques[i].tamano);
    }
}




/*asigna bloques de memoria*/
void
asignarBloques(ssize_t tamano, const char *nombreArchivo, key_t clave, int descriptor, const char *modo,
               void *direccion,
               int *numeroBloques, struct bloqab tablaBloques[], int opcion) {

    if (*numeroBloques < MAX_BLOQUES) { //si no se ha alcanzado el maximo de bloques
        strncpy(tablaBloques[*numeroBloques].modoMemoria, modo, //asignamos variables
                sizeof(tablaBloques[*numeroBloques].modoMemoria));
        tablaBloques[*numeroBloques].tamano = tamano;
        tablaBloques[*numeroBloques].tiempo = time(NULL);
        tablaBloques[*numeroBloques].direccionMemoria = direccion;

        if (opcion == OPCION_SHARED) {
            tablaBloques[*numeroBloques].clave = (key_t) clave;
        } else if (opcion == OPCION_MMAP) {
            strncpy(tablaBloques[*numeroBloques].nombreArchivo, nombreArchivo,
                    sizeof(tablaBloques[*numeroBloques].nombreArchivo));
            tablaBloques[*numeroBloques].descriptor = descriptor;
            printf("Archivo %s mapeado en %p\n", nombreArchivo, direccion);
        } else if (opcion == OPCION_MALLOC) {
            printf("Asignados %zd bytes en %p\n", tamano, direccion);
        }

        ++*numeroBloques; //sumamos numero de bloques
    } else { //si se ha alcanzado el maximo de bloques
        printf("Maximo de bloques alcanzados\n");
        return;
    }
}





void desasignarBloques(int tamanoEliminar, const char *nombreArchivo, key_t clave, int *numeroBloques,
                       struct bloqab tablaBloques[], int opcion) {
    int posicion = -1; //posicion para saber si se encuentra el bloque

    for (int i = 0; i < *numeroBloques; i++) { //iteramos sobre tabla de bloques
        if ((opcion == OPCION_MALLOC && tablaBloques[i].tamano == tamanoEliminar) || //si se encuentra el bloque
            (opcion == OPCION_MMAP && strcmp(tablaBloques[i].nombreArchivo, nombreArchivo) == 0) ||
            (opcion == OPCION_SHARED && tablaBloques[i].clave == clave)) {
            posicion = i; //guardamos posicion
            break;
        }
    }

    if (posicion != -1) { //si se ha encontrado el bloque
        if (opcion == OPCION_MALLOC) { //se libera memoria correspondiente
            free(tablaBloques[posicion].direccionMemoria);
            printf("Liberados %zd bytes en %p\n", tablaBloques[posicion].tamano,
                   tablaBloques[posicion].direccionMemoria);
        }
        if (opcion == OPCION_MMAP) {
            munmap(tablaBloques[posicion].direccionMemoria, tablaBloques[posicion].tamano);
            printf("Fichero %s desmapeado de %p\n", tablaBloques[posicion].nombreArchivo,
                   tablaBloques[posicion].direccionMemoria);
        }
        if (opcion == OPCION_SHARED) {
            shmdt(tablaBloques[posicion].direccionMemoria);
            printf("Memoria compartida clave %d desasignada de %p\n", tablaBloques[posicion].clave,
                   tablaBloques[posicion].direccionMemoria);
        }
        if (posicion < *numeroBloques - 1) {
            for (int j = posicion; j < *numeroBloques - 1; j++) { //bucle para desplazar los elementos en la lista
                tablaBloques[j] = tablaBloques[j + 1];
            }
        }

        --*numeroBloques; //restamos numero de bloques
    } else { //si no se ha encontrado el bloque
        if (opcion == OPCION_MALLOC) printf("No se encuentra bloque malloc con ese tamaño\n");
        if (opcion == OPCION_MMAP) printf("No se encuentra bloque mmap con ese nombre\n");
        if (opcion == OPCION_SHARED) printf("No se encuentra bloque shared con esa clave\n");
    }
}




/*lista bloques de memoria*/
void listarBloques(int numeroBloques, struct bloqab tablaBloques[], int opcion) {
    pid_t pid = getpid(); //obtiene pid

    if (opcion == -1) printf("**Lista de bloques asignados para el proceso %d\n", pid);
    if (opcion == OPCION_MALLOC) printf("**Lista de bloques asignados malloc para el proceso %d\n", pid);
    if (opcion == OPCION_MMAP) printf("**Lista de bloques asignados mmap para el proceso %d\n", pid);
    if (opcion == OPCION_SHARED) printf("**Lista de bloques asignados shared para el proceso %d\n", pid);

    for (int i = 0; i < numeroBloques; i++) { //iteramos sobre tabla de bloques
        time_t tiempo = tablaBloques[i].tiempo;
        struct tm *fechaSI;
        fechaSI = localtime(&tiempo);
        int dia = fechaSI->tm_mday, mes = fechaSI->tm_mon + 1, ano =
                fechaSI->tm_year + 1900, hora = fechaSI->tm_hour, min = fechaSI->tm_min, seg = fechaSI->tm_sec;

        if (opcion == OPCION_MALLOC || opcion == -1) //imprimimos segun opcion
            if (strcmp(tablaBloques[i].modoMemoria, "malloc") == 0)
                printf("\t%15p  %5zd %02d/%02d/%04d %02d:%02d:%02d %s\n",
                       tablaBloques[i].direccionMemoria,
                       tablaBloques[i].tamano, dia, mes, ano, hora, min, seg, tablaBloques[i].modoMemoria);
        if (opcion == OPCION_MMAP || opcion == -1)
            if (strcmp(tablaBloques[i].modoMemoria, "mmap") == 0)
                printf("\t%15p  %5zd %02d/%02d/%04d %02d:%02d:%02d %-5s  (descriptor %d)\n",
                       tablaBloques[i].direccionMemoria,
                       tablaBloques[i].tamano, dia, mes, ano, hora, min, seg, tablaBloques[i].nombreArchivo,
                       tablaBloques[i].descriptor);
        if (opcion == OPCION_SHARED || opcion == -1)
            if (strcmp(tablaBloques[i].modoMemoria, "shared") == 0)
                printf("\t%15p  %5zd %02d/%02d/%04d %02d:%02d:%02d %-6s (key %d)\n",
                       tablaBloques[i].direccionMemoria,
                       tablaBloques[i].tamano, dia, mes, ano, hora, min, seg, "shared", tablaBloques[i].clave);
    }
}



/*asigna o desasigna memoria malloc*/
void mallocc(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]) {
    if (tr[1] == NULL) { //si tr[1] es nulo
        listarBloques(*numeroBloques, tablaBloques, OPCION_MALLOC);
    } else {
        if (strcmp(tr[1], "-free") == 0) //si tr[1] es -free
            if (tr[2] == NULL) printf("Falta tamaño archivo\n");
            else
                desasignarBloques(atoi(tr[2]), NULL, -1, numeroBloques, tablaBloques,
                                  OPCION_MALLOC); //llama a funcion para desasignar
            else {
                ssize_t tamano = atoi(tr[1]); //obtiene tamaño
                void *creacionMalloc = malloc(tamano); //crea malloc
                if (tamano == 0) { //excepcion si tamaño es 0
                    printf("No se asignan bloques de 0 bytes\n");
                    return;
                }
                if (creacionMalloc == NULL) //excepcion si malloc es nulo
                    perror("Error al asignar bloque de memoria");
                else {
                    asignarBloques(tamano, NULL, -1, 0, "malloc", creacionMalloc, numeroBloques, tablaBloques,
                                   OPCION_MALLOC); //llama a funcion para asignar
                }
            }
    }
}





/*elimina clave de memoria compartida*/
int eliminarclave(key_t clave) {
    int id;

    if ((id = shmget(clave, 0, 0)) == -1) { //obtiene id
        perror("Error al obtener ID del segmento de memoria compartida");
        return errno;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1) { //elimina clave
        perror("Error al eliminar segmento de memoria compartida");
        return errno;
    }

    printf("Clave %d eliminada del sistema\n", (int) clave);
    return 0;
}


void *ObtenerMemoriaShmget(key_t clave, size_t tam) {
    void *p;
    int aux, id, flags = 0777;
    struct shmid_ds s;

    if (tam != 0) /*tam distito de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;
    if (clave == IPC_PRIVATE) { /*no nos vale*/
        errno = EINVAL;
        return NULL;
    }
    if ((id = shmget(clave, tam, flags)) == -1)
        return (NULL);
    if ((p = shmat(id, NULL, 0)) == (void *) -1) {
        aux = errno;
        if (tam != 0)
            shmctl(id, IPC_RMID, NULL);
        errno = aux;
        return (NULL);
    }
    shmctl(id, IPC_STAT, &s);
    return (p);
}

/*asigna o desasigna memoria compartida*/
void SharedCreate(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]) {
    key_t cl;
    size_t tam;
    void *p;

    if (tr[1] == NULL) { //si tr[1] es nulo
        listarBloques(*numeroBloques, tablaBloques, OPCION_SHARED);
        return;
    }

    if (strcmp(tr[1], "-free") == 0) { //si tr[1] es -free
        if (tr[2] != NULL) { //si tr[2] no es nulo
            key_t claveEliminar = (key_t) strtoul(tr[2], NULL, 10);
            desasignarBloques(0, NULL, claveEliminar, numeroBloques, tablaBloques,
                              OPCION_SHARED); //llama a funcion para desasignar
        } else {
            printf("Falta parametros\n");
        }
    } else if (strcmp(tr[1], "-delkey") == 0) { //si tr[1] es -delkey
        if (tr[2] == NULL) {
            printf("Faltan parametros\n");
            return;
        }
        eliminarclave((key_t) strtoul(tr[2], NULL, 10)); //llama a funcion para eliminar clave
    } else if (strcmp(tr[1], "-create") == 0) { //si tr[1] es -create
        if (tr[2] == NULL || tr[3] == NULL) { //si tr[2] o tr[3] son nulos
            printf("Faltan parametros\n");
            return;
        }
        cl = (key_t) strtoul(tr[2], NULL, 10);
        tam = (size_t) strtoul(tr[3], NULL, 10);
        if (tam == 0) {
            printf("No se asignan bloques de 0 bytes\n");
            return;
        }
        if ((p = ObtenerMemoriaShmget(cl, tam)) != NULL) {
            asignarBloques(tam, NULL, cl, 0, "shared", p, numeroBloques, tablaBloques,
                           OPCION_SHARED); //llama a funcion para asignar
            printf("Asignados %lu bytes en %p\n", tam, p);
        } else printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) cl, strerror(errno));
    } else { //si tr[1] no es nulo
        cl = (key_t)strtoul(tr[1], NULL, 10);
        size_t tamanoObtenido = 0;

        p = ObtenerMemoriaShmget(cl, 0); // Busca la memoria compartida existente
        if (p != NULL) {
            struct shmid_ds shmid_struct;
            if (shmctl(shmget(cl, 0, 0), IPC_STAT, &shmid_struct) != -1) {
                tamanoObtenido = shmid_struct.shm_segsz; // Obtiene el tamaño de la memoria compartida
                asignarBloques(tamanoObtenido, NULL, cl, 0, "shared", p, numeroBloques, tablaBloques, OPCION_SHARED);
                printf("Memoria compartida clave %lu en %p\n", (unsigned long)cl, p);
            } else {
                printf("Error al obtener información sobre memoria compartida clave %lu: %s\n", (unsigned long)cl, strerror(errno));
            }
        } else {
            printf("Imposible encontrar la memoria compartida clave %lu: %s\n", (unsigned long)cl, strerror(errno));
        }


    }
}

void *MapearFichero(const char *fichero, int protection, int *numeroBloques, struct bloqab tablaBloques[]) {
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p = NULL;

    if (protection & PROT_WRITE)
        modo = O_RDWR;
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1) //obtiene informacion del archivo
        return NULL;
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED) { //mapea archivo
        return NULL;
    } else {
        asignarBloques(s.st_size, fichero, -1, df, "mmap", p, numeroBloques, tablaBloques,
                       OPCION_MMAP); //llama a funcion para asignar
    }
    return p;
}

/*asigna o desasigna memoria mmap*/
void CmdMmap(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]) {
    char *perm;
    void *p;
    int protection = 0;
    if (tr[1] == NULL) { //si tr[1] es nulo
        listarBloques(*numeroBloques, tablaBloques, OPCION_MMAP);
    } else {
        if (strcmp(tr[1], "-free") == 0) //si tr[1] es -free
            if (tr[2] == NULL) printf("Falta nombre archivo\n");
            else
                desasignarBloques(0, tr[2], -1, numeroBloques, tablaBloques,
                                  OPCION_MMAP); //llama a funcion para desasignar
        else {
            if ((perm = tr[2]) != NULL && strlen(perm) < 4) {
                if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
                if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
                if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
            }
            p = MapearFichero(tr[1], protection, numeroBloques, tablaBloques); //llama a funcion para mapear
            if (p == NULL) //excepcion si p es nulo
                perror("Imposible mapear fichero");
        }
    }
}



void Do_MemPmap() { /*sin argumentos*/
    pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
    char elpid[32];
    char *argv[4] = {"pmap", elpid, NULL};

    sprintf(elpid, "%d", (int) getpid());
    if ((pid = fork()) == -1) {
        perror("Imposible crear proceso");
        return;
    }
    if (pid == 0) { /*proceso hijo*/
        if (execvp(argv[0], argv) == -1)
            perror("cannot execute pmap (linux, solaris)");

        argv[0] = "vmmap";
        argv[1] = "-interleave";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1) /*probamos vmmap Mac-OS*/
            perror("cannot execute vmmap (Mac-OS)");

        argv[0] = "procstat";
        argv[1] = "vm";
        argv[2] = elpid;
        argv[3] = NULL;
        if (execvp(argv[0], argv) == -1)/*No hay pmap, probamos procstat FreeBSD */
            perror("cannot execute procstat (FreeBSD)");

        argv[0] = "procmap", argv[1] = elpid;
        argv[2] = NULL;
        if (execvp(argv[0], argv) == -1)  /*probamos procmap OpenBSD*/
            perror("cannot execute procmap (OpenBSD)");

        exit(1);
    }
    waitpid(pid, NULL, 0);
}

/*muestra informacion de memoria*/
void mem(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]) {
    int a = 0, b = 1, c = 2; //variables locales
    static int g = 0, h = 1, ii = 2, Ng, Nh, Ni; //variables estaticas
    void (*PF1)() = &Do_MemPmap, (*PF2)() = (void (*)(void)) &recurse, (*PF3)() = (void (*)(void)) &memdump; //punteros a funciones
    void (*LF1)() = (void (*)()) &getc, (*LF2)() = (void (*)()) &getpid, (*LF3)() = (void (*)()) &getcwd;


    bool bol = false;
    int i = 1;
    do { //bucle para imprimir
        if (tr[1] == NULL || strcmp(tr[i], "-all") == 0) { //si tr[1] es nulo o tr[i] es -all
            bol = true; //bol es true
        }
        if (bol || strcmp(tr[i], "-vars") == 0) { //si bol es true o tr[i] es -vars
            printf("Variables locales\t %p\t %p\t %p\n" //imprime variables
                   "Variables globales\t %p\t %p\t %p\n"
                   "Var (N.I.)globales\t %p\t %p\t %p\n"
                   "Variables staticas\t %p\t %p\t %p\n"
                   "Var (N.I.)staticas\t %p\t %p\t %p\n", (void *) &a, (void *) &b, (void *) &c, (void *) &Globald,
                   (void *) &Globale, (void *) &Globalf, (void *) &GlobalNd, (void *) &GlobalNe, (void *) &GlobalNf,
                   (void *) &g, (void *) &h, (void *) &ii, (void *) &Ng, (void *) &Nh, (void *) &Ni);
        }
        if (bol || strcmp(tr[i], "-funcs") == 0) { //si bol es true o tr[i] es -funcs
            printf("Funciones programa\t %p\t %p\t %p\n" //imprime funciones
                   "Funciones libreria\t %p\t %p\t %p\n", (void *) &PF1, (void *) &PF2, (void *) &PF3, (void *) &LF1,
                   (void *) &LF2, (void *) &LF3);
        }
        if (bol || strcmp(tr[i], "-blocks") == 0) { //si bol es true o tr[i] es -blocks
            listarBloques(*numeroBloques, tablaBloques, -1); //llama a funcion para listar bloques
        }
        if (!bol && strcmp(tr[i], "-pmap") == 0) { //si bol es false y tr[i] es -pmap
            Do_MemPmap(); //llama a funcion para mostrar informacion de memoria
        }
        bol = false;
        ++i; //suma uno a i
    } while (tr[i] != NULL); //mientras tr[i] no sea nulo

}


void memdump(char *tr[]) {
    if (tr[1] == NULL) {
        return;
    }
    void *addr = (void *) strtol(tr[1], NULL, 16); //convierte cadena de caracteres en puntero
    size_t cont;

    if (tr[2] == NULL) { //decide que valor tendra cont
        cont = 25;
    } else if (atoi(tr[2]) < 0) {
        printf("Cont no puede ser negativo\n");
        return;
    } else {
        cont = (size_t) strtol(tr[2], NULL, 10);
    }

    printf("Volcando %zu bytes desde la dirección %p\n", cont, addr);

    unsigned char *p = (unsigned char *) addr; //convierte puntero en unsigned char

    int lineasCompletas = cont / 25;
    int resto = cont % 25;

    for (int i = 0; i < lineasCompletas; ++i) { //bucle para imprimir
        printf(" ");
        for (size_t j = i * 25; j < (i + 1) * 25; ++j) {
            if (j < cont) { //si j es menor que cont
                if (isprint(p[j])) printf("%c   ", p[j]); //si es imprimible
                else if (p[j] == '\n') printf("\\n"); //si es salto de linea
                else printf("    "); //si no es imprimible
            }
        }
        printf("\n");

        for (size_t j = i * 25; j < (i + 1) * 25; ++j) {
            if (j < cont) printf("%02X  ", p[j]); //si j es menor que cont
        }
        printf("\n");
    }

    if (resto != 0) { //si resto no es 0
        printf(" ");
        for (size_t i = lineasCompletas * 25; i < lineasCompletas * 25 + resto; ++i) {
            if (isprint(p[i])) printf("%c   ", p[i]); //si es imprimible
            else if (p[i] == '\n') printf("\\n"); //si es salto de linea
            else printf("    ");
        }
        printf("\n");
        for (size_t i = lineasCompletas * 25; i < lineasCompletas * 25 + resto; ++i) {
            printf("%02X  ", p[i]); //si es imprimible
        }
        printf("\n");
    }
}



/*ejecuta funcion recursiva n veces*/
void recurse(int n) {
    char automatico[TAMANO];
    static char estatico[TAMANO];
    printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, &n, automatico, estatico);

    if (n > 0)
        recurse(n - 1);
}
//jkfdk
void LlenarMemoria(void *p, size_t cont, unsigned char byte) {
    unsigned char *arr = (unsigned char *) p;
    size_t i;

    for (i = 0; i < cont; i++)
        arr[i] = byte;
}

/*llena memoria*/
void memfill(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]) {
    size_t cont;

    if (tr[1] == NULL || tr[2] == NULL) { //si tr[1] o tr[2] son nulos
        printf("Faltan parametros\n");
        return;
    }

    void *addr = (void *) strtol(tr[1], NULL, 16); //convierte cadena de caracteres en puntero
    cont = (size_t) strtol(tr[2], NULL, 10); //cont es igual a tr[2]
    unsigned char byte; //byte es unsigned char

    if (tr[3] == NULL) { //decide que valor tendra byte
        byte = 'A';
    } else {
        byte = tr[3][0];
    }
    printf("Llenando %zu bytes de memoria con el byte %c(%02x) a partir de la direccion %s\n", cont, byte, byte,
           tr[1]);
    LlenarMemoria(addr, cont, byte); //llama a funcion para llenar memoria

}