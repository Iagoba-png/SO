/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba  iago.alcoba@udc.es
    GRUPO: 1.3
*/


#include "firstshellfunctions.h"
#include "list.h"

/*Muestra los nombres y logins*/
void authors(char *tr[]) {
    if (tr[1] == NULL) {
        printf("Alejandro Suárez, a.suarez4@udc.es\nIago Bescansa Alcoba, iago.alcoba@udc.es\n");
    } else {
        if (strcmp(tr[1], "-l") == 0) {
            printf("a.suarez4@udc.es \niago.alcoba@udc.es\n");
        }
        if (strcmp(tr[1], "-n") == 0) {
            printf("Alejandro Suárez García \nIago Bescansa Alcoba\n");
        }
    }
}

/*Muestra el pid */
void pid(char *tr[]) {
    if (tr[1] == NULL) { //si no se proporciona argumentos
        pid_t pidActual = getpid(); //obtiene pid actual y lo almacena
        printf("Pid shell: %d\n", pidActual);
    } else {
        if (strcmp(tr[1], "-p") == 0) {
            pid_t pidPadre = getppid(); //obtiene pid padre y lo almacena
            printf("Pid padre shell: %d\n", pidPadre);
        }
    }
}

/*muestra la fecha actual*/
void date(char *tr[]) {
    time_t ahora;
    struct tm *info;
    time(&ahora);
    info = localtime(&ahora);

    int day = info->tm_mday, mon = info->tm_mon + 1, year = info->tm_year + 1900;
    int hour = info->tm_hour, min = info->tm_min, sec = info->tm_sec;

    // ---- CASOS ----
    if (strcmp(tr[0], "date") == 0 && tr[1] == NULL) {
        // "date" → fecha y hora
        printf("%02d/%02d/%04d\n", day, mon, year);
        printf("%02d:%02d:%02d\n", hour, min, sec);
    } else if (strcmp(tr[0], "date") == 0 && strcmp(tr[1], "-d") == 0) {
        // "date -d" → solo fecha
        printf("%02d/%02d/%04d\n", day, mon, year);
    } else if (strcmp(tr[0], "date") == 0 && strcmp(tr[1], "-t") == 0) {
        // "date -t" → solo hora
        printf("%02d:%02d:%02d\n", hour, min, sec);
    } else if (strcmp(tr[0], "hour") == 0 && tr[1] == NULL) {
        // "hour" → solo hora
        printf("%02d:%02d:%02d\n", hour, min, sec);
    } else {
        printf("Uso: date [-d|-t] | hour\n");
    }
}
/*muestra directorio actual o cambia directorio*/
void chdirr(char *tr[]) {
    if (tr[1] == NULL) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n", cwd); //se obtiene el dir actual, si tiene exito se muestra
        else {
            perror("Error al acceder al directorio");
        }
    } else {
        if (chdir(tr[1]) == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                printf("Directorio cambiado con éxito a %s\n",
                       cwd); //se obtiene el dir actual, si tiene exito se muestra cambio de dir
        } else {
            perror("Error al cambiar de directorio");
        }
    }
}

/*muestra informacion de la maquina*/
void infosys(char *tr[]) {
    struct utsname informacionSistema; //declaramos variable como 'struct utsname' para almacenar el tipo de maquina, version...
    if (tr[1] == NULL) {
        if (uname(&informacionSistema) == 0) //obtiene informacion del sistema y la almacena, si tiene exito se muestra
            printf("%s %s, SO: %s %s %s\n", informacionSistema.nodename, informacionSistema.machine,
                   informacionSistema.sysname, informacionSistema.release, informacionSistema.version);
        else perror("Error en informacion sistema");
    } else {
        if (strcmp(tr[1], "uname") == 0) {
            if (uname(&informacionSistema) ==
                0) //obtiene informacion del sistema y la almacena, si tiene exito se muestra
                printf("%s %s, SO: %s %s %s\n", informacionSistema.nodename, informacionSistema.machine,
                       informacionSistema.sysname, informacionSistema.release, informacionSistema.version);
            else perror("Error en informacion sistema");
        }
    }
}

/*muestra informacion de comandos*/
void help(char *tr[]) {
    struct comando comandos[MAX_COMANDOS] = {
            {"authors",  "[-l|-n]    Muestra el nombre y/o login de los creadores"},
            {"getpid",      "[-p]   Muestra el pid del shell o del proceso padre"},
            {"chdir",    "[dir]    Muestra el directorio actual o cambia de directorio"},
            {"date",     "[hour|-t|-d]      Muestra la fecha y hora actual,solo la hora o solo la fecha"},
            {"historic",     "[N|-N] [-clear|-count]  Muestra el historial, el comando N o los últimos -N comandos. Se puede eliminar el historial o ver el numero de comandos usados "},
            {"comand",   "N   Repite el N comando del historial de comandos"},
            {"open",     "[file] mode       Abre un archivo en un determinado mode"},
            {"close",    "[df]     Cierra un archivo"},
            {"dup",      "[df]      Duplica un archivo"},
            {"listopen", "      Lista los archivos abiertos del shell"},
            {"infosys",  "       Muestra informacion del sistema"},
            {"exit",     "      Finalizacion ejecucion shell"},
            {"quit",     "      Finalización ejecución shell"},
            {"bye",      "       Finalización ejecución shell"},

    };
    if (tr[1] == NULL) {
        for (int i = 0; i < MAX_COMANDOS; i++) { //bucle que recorre comandos para mostrar el nombre
            printf("%s ", comandos[i].nombreComando);
            if (i == MAX_COMANDOS - 1) printf("\n");
        }
    } else {

        int comandoEncontrado = 0;
        for (int i = 0; i < MAX_COMANDOS; i++) { //bucle que recorre comandos para mostrar informacion de un comando
            if (strcmp(tr[1], comandos[i].nombreComando) == 0) {
                printf("%s %s\n", comandos[i].nombreComando, comandos[i].explicacionComando);
                comandoEncontrado++;
            }
        }
        if (comandoEncontrado == 0)printf("Comando %s no encontrado\n", tr[1]);
    }
}

/*muestra historial de comandos*/
void printComands(tList L) {
    tPoss p = L.start; //declara p y lo inicializa al primer nodo de la lista
    int i = 1;
    while (p != NULL) {
        printf("%d: %s", i, p->it);
        ++i;
        p = p->next; //avanza al siguiente nodo
    }
}

/*muestra historial de n comandos*/
void printNComands(tList L, int n) {
    tPoss p = L.start; //declara p y lo inicializa al primer nodo de la lista
    n = abs(n);
    for (int i = 1; i <= n; ++i) { //bucle que itera en la lista hasta n
        if (p == NULL) { break; } //si llega a final de lista sale de bucle
        if (i == n) {
            printf("%d: %s", i, p->it);
        }
        p = p->next; //avanza al siguiente nodo
    }
}

/* imprime los últimos n comandos */
void printLastNComands(tList L, int n) {
    int total = lengL(L); // cuenta los nodos de la lista
    int skip = (total > n) ? total - n : 0;

    tPoss p = L.start;
    int i = 1;
    while (p != NULL) {
        if (i > skip) {
            printf("%d: %s", i, p->it);
        }
        p = p->next;
        i++;
    }
}


/*muestra historial de comandos*/
void hist(char *tr[], tList *L) {
    if (tr[1] == NULL) {
        printComands(*L);  // muestra historial

    } else if (strcmp(tr[1], "-clear") == 0) {
        clearList(L);      // limpia historial

    }else if (strcmp(tr[1], "-count") == 0) {
        printf("Número de comandos en el historial: %d\n", lengL(*L));

    } else {
        int n = (int) strtol(tr[1], NULL, 10);

        if (n > 0) {
            // historic N → mostrar el comando número N (o repetirlo si quieres)
            // ahora mismo solo lo mostramos:
            printNComands(*L, n);

        } else if (n < 0) {
            // historic -N → mostrar los últimos N
            printLastNComands(*L, -n);

        } else {
            printf("Uso: historic [N|-N|-clear]\n");
        }
    }
}


/*lista ficheros abiertos*/
void listarFicherosAbiertos(int numeroFicheros, struct fichab tablaFicheros[]) {
    for (int i = 0; i < numeroFicheros - 1; i++) { //bucle que ordena la tabla de archivos
        for (int j = 0; j < numeroFicheros - 1 - i; j++) {
            if (tablaFicheros[j].descrip > tablaFicheros[j + 1].descrip) { //verifica si el descriptor actual es mayor que el siguiente descriptor
                struct fichab cambioPos = tablaFicheros[j]; //incializa struct que almacena contenido de posj
                tablaFicheros[j] = tablaFicheros[j + 1]; //intercambia posiciones
                tablaFicheros[j + 1] = cambioPos;
            }
        }

    }
    for (int i = 0; i < numeroFicheros; i++) {
        const char *modos = ""; //se declara modos como un puntero a un string vacio
        int modo = tablaFicheros[i].modo; //obtenemos el modo de apertura desde tablaficheros
        if (modo != 0) {
            if (modo & O_RDWR) modos = "O_RDWR";
            else if (modo & O_RDONLY) modos = "O_RDONLY";
            else if (modo & O_WRONLY) modos = "O_WRONLY";
            else if (modo & O_APPEND) modos = "O_APPEND";
            else if (modo & O_CREAT) modos = "O_CREAT";
            else if (modo & O_EXCL) modos = "O_EXCL";
            else if (modo & O_TRUNC) modos = "O_TRUNC";
        }
        printf("descriptor: %d -> %s %s\n", tablaFicheros[i].descrip, tablaFicheros[i].nombre,
               modos); //muestra informacion
    }
}

/*añade ficheros a la lista*/
void anadirFicherosAbiertos(const char *nombreArchivo, int mode, int df, int *numeroFicheros,
                            struct fichab tablaFicheros[]) {
    if (*numeroFicheros < MAX_FICHEROS) { //si no excede la capacidad maxima de ficheros
        strncpy(tablaFicheros[*numeroFicheros].nombre, nombreArchivo,
                sizeof(tablaFicheros[*numeroFicheros].nombre)); //copia el nombre del archivo en la tablaFicheros en la posicion numeroFicheros
        tablaFicheros[*numeroFicheros].modo = mode; //almacenamos el modo
        tablaFicheros[*numeroFicheros].descrip = df; //almacenamos el descriptor
        ++*numeroFicheros;
    } else return;
}

/*añade archivos estandar*/
void parentProcess(int *numOpenCommands, int *numeroFicheros, struct fichab tablaFicheros[]) {
    int stdin_fileno = fileno(stdin); //define los descriptores de los archivos
    int stdout_fileno = fileno(stdout);
    int stderror_fileno = fileno(stderr);
    if (*numOpenCommands == 0) {
        int dfStdIn = stdin_fileno; //copia descriptor de archivo
        int dfStdout = stdout_fileno;
        int dfStError = stderror_fileno;
        anadirFicherosAbiertos("entrada estandar", O_RDWR, dfStdIn, numeroFicheros,
                               tablaFicheros); //se añaden los archivos a la lista
        anadirFicherosAbiertos("salida estandar", O_RDWR, dfStdout, numeroFicheros, tablaFicheros);
        anadirFicherosAbiertos("error estandar", O_RDWR, dfStError, numeroFicheros, tablaFicheros);
        ++*numOpenCommands;
    }
}

/*abre archivos*/
void Cmd_open(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]) {
    if (tr[1] == NULL) { //si tiene exito, se muestra la lista de archivos
        listarFicherosAbiertos(*numeroFicheros, tablaFicheros);
        return;
    } else {
        int mode = 0;
        if (tr[2] != NULL) {
            if (strcmp(tr[2], "cr") == 0) mode |= O_CREAT;
            else if (strcmp(tr[2], "ex") == 0) mode |= O_EXCL;
            else if (strcmp(tr[2], "ro") == 0) mode |= O_RDONLY;
            else if (strcmp(tr[2], "wo") == 0) mode |= O_WRONLY;
            else if (strcmp(tr[2], "rw") == 0) mode |= O_RDWR;
            else if (strcmp(tr[2], "ap") == 0) mode |= O_APPEND;
            else if (strcmp(tr[2], "tr") == 0) mode |= O_TRUNC;
            else mode = 0;
        }
        int df = open(tr[1], mode, 0777); //abre el archivo en el modo especificado
        if (df == -1) { perror("Imposible abrir el archivo"); } //si no tiene exito muestra error
        else {
            anadirFicherosAbiertos(tr[1], mode, df, numeroFicheros, tablaFicheros); //añade ficheros a la lista
            printf("Anadida entrada a la tabla ficheros abiertos\n");
        }
    }
}

/*lista ficheros abiertos*/
void listOpen(int numeroFicheros, struct fichab tablaFicheros[]) {
    listarFicherosAbiertos(numeroFicheros, tablaFicheros);
}

/*elimina fichero de lista de archivos*/
void eliminarDeFicherosAbiertos(int df, int *numeroFicheros, struct fichab tablaFicheros[]) {
    for (int i = 0; i < *numeroFicheros; i++) {
        if (tablaFicheros[i].descrip == df) { //si el valor descriptor en posicion i coincide con df
            close(df); //cierra el archivo
            for (int j = i; j < *numeroFicheros - 1; j++) { //bucle para desplazar los elementos en la lista
                tablaFicheros[j] = tablaFicheros[j + 1];
            }
            --*numeroFicheros;
        }
    }
}

/*cierra archivos*/
void Cmd_close(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]) {
    int df;
    if (tr[1] == NULL) {
        listarFicherosAbiertos(*numeroFicheros, tablaFicheros); //si tiene exito, se muestra la lista de archivos
        return;
    }
    df = atoi(tr[1]);
    if (df < 0) {
        listarFicherosAbiertos(*numeroFicheros, tablaFicheros);
        return;
    }
    if (close(df) == -1) //si no se puede cerrar, muestra error
        perror("Imposible cerrar descriptor");
    else {
        eliminarDeFicherosAbiertos(df, numeroFicheros, tablaFicheros); //si se cierra, lo elimina de la lista
        printf("Archivo eliminado con éxito \n");
    }
}

/*duplica el descriptor de un archivo*/
void Cmd_dup(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]) {
    int df, duf;
    int i = 0;
    if (tr[1] == NULL || (df = atoi(tr[1])) < 0 ||
        df >
        *numeroFicheros) { //si no se proporciona descritpor, es negativo o menor que el numero de ficheros abiertos
        listarFicherosAbiertos(*numeroFicheros, tablaFicheros);
        return;
    }
    if ((duf = dup(df)) == -1) //duplica el descriptor, si no tiene exito muestra error
        perror("Imposible cerrar descriptor");
    else {
        while (i < *numeroFicheros) {
            if (df == tablaFicheros[i].descrip) { break; } //encuentra el i del descriptor en la lista
            i++;
        }
        char c[200];
        sprintf(c, "dup %d ", df);
        strcat(c, tablaFicheros[i].nombre); //crea nuevo nombre para descriptor duplicado
        anadirFicherosAbiertos(c, tablaFicheros[i].modo, duf, numeroFicheros, tablaFicheros); //lo agrega a la lista
    }
}


/*escribe una cadena en el archivo abierto con descriptor df*/
void writestr(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]) {
    if (tr[1] == NULL || tr[2] == NULL) {
        perror("Uso: writestr <descriptor> <texto>");
        return;
    }

    int df = atoi(tr[1]);  // descriptor real (como usa open/close)
    int indice = -1;

    // Buscar el descriptor real en la tabla
    for (int i = 0; i < *numeroFicheros; i++) {
        if (tablaFicheros[i].descrip == df) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        perror("Descriptor no encontrado en tabla");
        return;
    }

    // Construir la cadena completa a escribir
    char buffer[1024] = "";
    for (int i = 2; tr[i] != NULL; i++) {
        strcat(buffer, tr[i]);
        if (tr[i + 1] != NULL) strcat(buffer, " ");
    }

    // Escribir en el archivo
    ssize_t bytes = write(df, buffer, strlen(buffer));
    if (bytes == -1)
        perror("Error al escribir en el archivo");
    else
        printf("Se escribieron %zd bytes en %s\n", bytes, tablaFicheros[indice].nombre);
}


void seek(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]) {
    if (tr[1] == NULL || tr[2] == NULL || tr[3] == NULL) {
        printf("Uso: lseek <descriptor> <offset> <set|cur|end>\n");
        return;
    }

    int df = atoi(tr[1]);
    off_t offset = atol(tr[2]);
    int referencia;

    // Buscar el descriptor en la tabla para asegurarnos de que está abierto
    int indice = -1;
    for (int i = 0; i < *numeroFicheros; i++) {
        if (tablaFicheros[i].descrip == df) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        perror("Descriptor no encontrado en tabla");
        return;
    }

    // Determinar tipo de referencia
    if (strcmp(tr[3], "set") == 0) referencia = SEEK_SET;
    else if (strcmp(tr[3], "cur") == 0) referencia = SEEK_CUR;
    else if (strcmp(tr[3], "end") == 0) referencia = SEEK_END;
    else {
        printf("Referencia invalida. Use: set | cur | end\n");
        return;
    }

    // Realizar el desplazamiento
    off_t nueva_pos = lseek(df, offset, referencia);
    if (nueva_pos == (off_t)-1) {
        perror("Error al reposicionar el puntero");
    } else {
        printf("Nuevo offset en %s: %ld\n", tablaFicheros[indice].nombre, (long)nueva_pos);
    }
}