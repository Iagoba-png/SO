/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/

#include "filefunctions.h"


/*muestra el dir actual*/
void obtenerDirectorioActual() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n", cwd); //se obtiene el dir actual, si tiene exito se muestra
    else perror("Error al acceder al directorio");
}

/*crea un archivo o directorio*/
void create(char *tr[], int *numerodirectorios, struct dirab tabladirectorios[]) {
    if (tr[1] == NULL) {
        obtenerDirectorioActual();
    } else {
        if (strcmp(tr[1], "-f") == 0) {
            if (tr[2] == NULL) {
                perror("Falta nombre archivo");
            } else {
                int fd = open(tr[2], O_CREAT | O_WRONLY, 0666);
                if (fd == -1) { //crea archivo, si no tiene exito muestra error
                    perror("Fichero no creado");
                } else {
                    close(fd);
                }
            }
        } else {
            if (mkdir(tr[1], 0755) == -1) { //crea directorio, si no tiene exito muestra error
                perror("Directorio no creado");
            } else {
                strncpy(tabladirectorios[*numerodirectorios].nombre, tr[1],
                        sizeof(tabladirectorios[*numerodirectorios].nombre)); //copia el nombre del fichero en la tabladeficheros en la posicion numerofichero
                ++*numerodirectorios;
            }
        }

    }
}


bool esDirectorioVacio(const char *path) {
    struct dirent *entrada; //struct para entrada a directorio
    DIR *directorio = opendir(path); //abre directorio

    if (directorio == NULL) return false;

    while ((entrada = readdir(directorio)) != NULL) { //bucle que itera en el directorio
        if ((strcmp(entrada->d_name, ".") != 0 &&
             strcmp(entrada->d_name, "..") != 0)) {
            closedir(directorio);
            return false; //si hay contenido en el directorio devuelve false
             }
    }
    closedir(directorio);
    return true;
}


void erase(char *tr[]) {
    if (tr[1] == NULL) {
        obtenerDirectorioActual();
        return;
    } else {
        int i = 1;
        bool continuar = true;
        do { //recorre los argumentos mientras continuar sea true
            if (tr[i] != NULL) {
                struct stat estadisticasarchdir; //struct para estadisticas de arch y dir
                if (stat(tr[i], &estadisticasarchdir) == -1)
                    perror("***Error al obtener informacion"); //se obtiene informacion, si no tiene exito muestra error
                else {
                    if (S_ISDIR(estadisticasarchdir.st_mode)) { //comprueba si es directorio
                        if (esDirectorioVacio(tr[i])) { //comprueba si esta vacio
                            if (rmdir(tr[i]) == 0)
                                printf("Directorio %s borrado con exito\n",
                                       tr[i]); //borra directorio, si tiene exito se muestra
                            else perror("Error al borrar directorio");
                        } else printf("Directorio %s no borrado ya que no se encuentra vacio\n", tr[i]);
                    } else {
                        if (unlink(tr[i]) == 0)
                            printf("Fichero %s borrado con exito\n", tr[i]); //borra archivo, si tiene exito se muestra
                        else perror("Error al borrar fichero");
                    }
                }
                i++;
            } else continuar = false;
        } while (continuar);
    }
}

void delrec(char *tr[]) {
    char cwd[1024];
    struct stat estadisticasarchdir; //struct para estadisticas de arch y dir
    if (tr[1] == NULL) {  //si el comando solo es delrec:
        if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s\n", cwd); //se obtiene el dir actual, si tiene exito se muestra
        else perror("Error al acceder al directorio");
        return;
    } else {
        int i = 1;
        bool continuar = true;
        bool rec = false;  //Variable que dtecta si se ha borrado el interior de algún directorio
        do {   //Recorremos los nombres a borrar del comando
            if (tr[i] != NULL) {
                if (stat(tr[i], &estadisticasarchdir) != -1) { //se obtiene informacion
                    if (S_ISDIR(estadisticasarchdir.st_mode)) { //comprueba si es directorio
                        if (!esDirectorioVacio(tr[i])) { //comprueba si esta vacio
                            DIR *directorio = opendir(tr[i]);
                            struct dirent *entrada;
                            char cad[PATH_MAX] = "delrec";
                            while ((entrada = readdir(directorio)) !=
                                   NULL) {  //Creo un String que sea otro comando delrec con el contenido del directorio a borrar
                                if (strcmp(entrada->d_name, ".") != 0 &&
                                    strcmp(entrada->d_name, "..") != 0) { //estos archivos no cuentan
                                    strcat(cad, " ");
                                    strcat(cad, entrada->d_name);
                                }
                            }
                            char *trozos[30];
                            for (int j = 0; j < 30; j++) {
                                trozos[j] = NULL;
                            }
                            int j = 1;
                            trozos[0] = strtok(cad, " \n\t");
                            while ((trozos[j] = strtok(NULL, " \n\t")) !=
                                   NULL) { //Trozeo el string del comando para poder pasarlo como parametro
                                j++;
                            }
                            closedir(directorio);
                            chdir(tr[i]);     //Entro en el directorio a borrar
                            delrec(trozos);  //Hago delREC con todos los archivos de dentro del directorio
                            chdir("../");     //Salgo del directorio ahora vacío
                            rec = true;

                        }
                    }
                }
                i++;
            } else continuar = false;
        } while (continuar);
        if (rec) {
            delrec(tr);
        }//Si se ha borrado el interior de algún fichero se repite el delrec actual pero con los ficheros vacios
        else {
            i = 1;
            continuar = true;
            do { //Recorremos los nombres a borrar del comando
                if (tr[i] != NULL) {
                    if (stat(tr[i], &estadisticasarchdir) ==
                        -1) { //se obtiene informacion, si no tiene exito muestra error
                        perror("***Error al obtener informacion");
                    } else {
                        if (S_ISDIR(estadisticasarchdir.st_mode)) { //comprueba si es directorio
                            if (esDirectorioVacio(tr[i])) { //comprueba si esta vacio
                                if (rmdir(tr[i]) == 0)
                                    printf("Directorio %s borrado con exito\n",
                                           tr[i]); //borra directorio, si tiene exito se muestra
                                else perror("Error al borrar directorio");
                            } else printf("Directorio %s no borrado ya que no se encuentra vacio\n", tr[i]);
                        } else {
                            if (unlink(tr[i]) == 0)
                                printf("Fichero %s borrado con exito\n",
                                       tr[i]); //borra archivo, si tiene exito se muestra
                            else perror("Error al borrar fichero");
                        }
                    }
                    i++;
                } else continuar = false;
            } while (continuar);
        }

    }
}




/*devuelve la letra del tipo de fichero*/
char LetraTF(mode_t m) {
    switch (m & S_IFMT) {
        case S_IFSOCK: return 's';
        case S_IFLNK:  return 'l';
        case S_IFREG:  return '-';
        case S_IFBLK:  return 'b';
        case S_IFDIR:  return 'd';
        case S_IFCHR:  return 'c';
        case S_IFIFO:  return 'p';
        default:       return '?';
    }
}

/*convierte el modo a una cadena de permisos tipo ls -l*/
char *ConvierteModo(mode_t m, char *permisos) {
    strcpy(permisos, "---------- ");
    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's';
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';
    return permisos;
}

/*establece los parametros del comando dir*/
void setdirparams(char *tr[], DirParams *params) {
    if (tr[1] == NULL) {
        printf("Uso: setdirparams [long|short|link|nolink|hid|nohid|reca|recb|norec]\n");
        return;
    }

    for (int i = 1; tr[i] != NULL; i++) {
        if (strcmp(tr[i], "long") == 0) params->longFormat = true;
        else if (strcmp(tr[i], "short") == 0) params->longFormat = false;
        else if (strcmp(tr[i], "link") == 0) params->showLinks = true;
        else if (strcmp(tr[i], "nolink") == 0) params->showLinks = false;
        else if (strcmp(tr[i], "hid") == 0) params->showHidden = true;
        else if (strcmp(tr[i], "nohid") == 0) params->showHidden = false;
        else if (strcmp(tr[i], "reca") == 0) params->recursion = 1;
        else if (strcmp(tr[i], "recb") == 0) params->recursion = 2;
        else if (strcmp(tr[i], "norec") == 0) params->recursion = 0;
        else printf("Parámetro desconocido: %s\n", tr[i]);
    }
}

/*muestra los parametros actuales*/
void getdirparams(DirParams *params) {
    printf("Formato:   %s\n", params->longFormat ? "long" : "short");
    printf("Links:     %s\n", params->showLinks ? "link" : "nolink");
    printf("Ocultos:   %s\n", params->showHidden ? "hid" : "nohid");
    printf("Recursión: ");
    if (params->recursion == 1) printf("reca\n");
    else if (params->recursion == 2) printf("recb\n");
    else printf("norec\n");
}


/*muestra el contenido de un directorio*/
void listarContenido(const char *ruta, DirParams *params, bool listarDirectorio) {
    DIR *dir;
    struct dirent *entrada;
    struct stat st;
    char path[PATH_MAX];

    if ((dir = opendir(ruta)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    if (listarDirectorio) printf("******%s\n", ruta);

    while ((entrada = readdir(dir)) != NULL) {
        if (!params->showHidden && entrada->d_name[0] == '.') continue;

        snprintf(path, sizeof(path), "%s/%s", ruta, entrada->d_name);
        lstat(path, &st);

        if (params->longFormat) {
            char permisos[12];
            ConvierteModo(st.st_mode, permisos);
            struct tm *tm_info = localtime(&st.st_mtime);
            printf("%10s%c %02d/%02d/%04d %8ld %s\n",
                   permisos,
                   LetraTF(st.st_mode),
                   tm_info->tm_mday,
                   tm_info->tm_mon + 1,
                   tm_info->tm_year + 1900,
                   (long) st.st_size,
                   entrada->d_name);
        } else if (params->showLinks && S_ISLNK(st.st_mode)) {
            char destino[PATH_MAX];
            ssize_t len = readlink(path, destino, sizeof(destino) - 1);
            if (len != -1) {
                destino[len] = '\0';
                printf("%8ld %s -> %s\n", (long) st.st_size, entrada->d_name, destino);
            } else printf("%8ld %s [error enlace]\n", (long) st.st_size, entrada->d_name);
        } else {
            printf("%8ld %s\n", (long) st.st_size, entrada->d_name);
        }
    }

    closedir(dir);
}

/*listado recursivo tipo reca (despues)*/
void listarRecursivoDespues(const char *ruta, DirParams *params) {
    DIR *dir;
    struct dirent *entrada;
    struct stat st;
    char path[PATH_MAX];

    if ((dir = opendir(ruta)) == NULL) {
        perror("No se pudo abrir el directorio");
        return;
    }

    printf("************%s\n", ruta);

    while ((entrada = readdir(dir)) != NULL) {
        if (!params->showHidden && entrada->d_name[0] == '.') continue;
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) continue;

        snprintf(path, sizeof(path), "%s/%s", ruta, entrada->d_name);
        if (lstat(path, &st) == 0) {
            listarContenido(ruta, params, false);
            if (S_ISDIR(st.st_mode)) {
                listarRecursivoDespues(path, params);
            }
        }
    }

    closedir(dir);
}

/*listado recursivo tipo recb (antes)*/
void listarRecursivoAntes(const char *ruta, DirParams *params) {
    DIR *dir;
    struct dirent *entrada;
    struct stat st;
    char path[PATH_MAX];

    if ((dir = opendir(ruta)) == NULL) {
        perror("No se pudo abrir el directorio");
        return;
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (!params->showHidden && entrada->d_name[0] == '.') continue;
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) continue;

        snprintf(path, sizeof(path), "%s/%s", ruta, entrada->d_name);
        if (lstat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            listarRecursivoAntes(path, params);
        }
    }

    printf("************%s\n", ruta);
    listarContenido(ruta, params, false);
    closedir(dir);
}


void dirComando(char *tr[], DirParams *params) {
    int i = 1;
    bool modoDirectorio = false;

    if (tr[1] != NULL && strcmp(tr[1], "-d") == 0) {
        modoDirectorio = true;
        i = 2;
    }

    if (tr[i] == NULL) {
        // Si no se pasa ruta, se lista el actual según recursión
        if (params->recursion == 1)
            listarRecursivoDespues(".", params);
        else if (params->recursion == 2)
            listarRecursivoAntes(".", params);
        else
            listarContenido(".", params, true);
        return;
    }

    while (tr[i] != NULL) {
        struct stat st;
        if (lstat(tr[i], &st) == -1) {
            perror("Error al obtener información");
        } else {
            if (params->recursion == 1)
                listarRecursivoDespues(tr[i], params);
            else if (params->recursion == 2)
                listarRecursivoAntes(tr[i], params);
            else if (S_ISDIR(st.st_mode) && modoDirectorio)
                listarContenido(tr[i], params, true);
            else
                listarContenido(tr[i], params, false);
        }
        i++;
    }
}



