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
                if (open(tr[2], O_CREAT, 0666) == -1) { //crea archivo, si no tiene exito muestra error
                    perror("Fichero no creado");
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



