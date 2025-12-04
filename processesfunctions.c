//
// Created by usuario on 29/11/25.
//

#include "processesfunctions.h"

/*muestra o cambia credenciales*/
void uid(char *tr[]) {
    if (tr[1] == NULL || strcmp(tr[1], "-get") == 0) { //si tr[1] es nulo o -get
        uid_t credencialReal = getuid(); //obtiene el id de usuario real
        struct passwd *informacionReal = getpwuid(credencialReal); //obtiene informacion de usuario a partir del id
        if (informacionReal == NULL) {
            perror("Error al obtener credencial real");
            return;
        }
        printf("Credencial real: %d, (%s)\n", credencialReal, informacionReal->pw_name); //muestra informacion

        uid_t credencialEfectiva = geteuid(); //obtiene el id de usuario efectivo
        struct passwd *informacionEfectiva = getpwuid(credencialEfectiva);
        if (informacionEfectiva == NULL) {
            perror("Error al obtener credencial efectiva");
            return;
        }
        printf("Credencial efectiva: %d, (%s)\n", credencialEfectiva, informacionEfectiva->pw_name);
    } else if (strcmp(tr[1], "-set") == 0) { //si tr[1] es -set
        if (tr[2] == NULL) return;
        if (strcmp(tr[2], "-l") == 0) { //si tr[2] es -l
            if (tr[3] == NULL) return;

            struct passwd *usuarioEscogido = getpwnam(tr[3]); //obtiene informacion del usuario a partir del nombre
            if (usuarioEscogido == NULL) {
                printf("Usuario no existente %s\n", tr[3]);
                return;
            }
            if (seteuid(usuarioEscogido->pw_uid) == -1) { //cambia el identificador de usuario efectivo
                perror("Error al cambiar credencial");
                return;
            }

        } else { //si tr[2] no es -l
            if (seteuid(atoi(tr[2])) == -1) { //cambia el identificador de usuario efectivo
                perror("Error al cambiar credencial");
                return;
            }

        }

    } else printf("Uso: uid [-get|-set] [-l] [id]\n");
}



/*muestra variables de entorno*/
void showenv(char *tr[], char *e[]) {
    extern char **environ; //Entorno real del proceso de kernel
    if (tr[1] != NULL) { //si tr[1] no es nulo
        if (strcmp(tr[1], "-environ") == 0) { //si tr[1] es -environ
            char **env = environ;
            int i = 0;

            while (*env != NULL) { //recorre las variables de entorno
                printf("%p->environ[%d]=(%p) %s\n", (void *) env, i, (void *) *env, *env); //muestra informacion
                env++; //avanza al siguiente elemento
                i++;
            }
        } else if (strcmp(tr[1], "-addr") == 0) { //si tr[1] es -addr
            printf("environ:   %p (almacenado en %p)\n", (void *) environ, (void *) &environ); //muestra informacion
            printf("main arg3: %p (almacenado en %p)\n", (void *) e,
                   (void *) &e);
        }
    } else {
        int i = 0;
        while (*e != NULL) {
            printf("%p->main arg3[%d]=(%p) %s\n", (void *) e, i, (void *) *e, *e);
            e++;
            i++;
        }
    }
}

/*ejecuta comandos externos*/
void comandosExternos(char *tr[], bool segundoPlano, struct jobsab tablaProcesos[], int *numeroJobs) {
    pid_t pid = fork(); //crear un proceso hijo

    if (pid < 0) { //si no se ha podido crear el proceso
        perror("Error al crear el proceso");
        return;
    } else if (pid == 0) { //si es el proceso hijo
        execvp(tr[0], tr); // ejecutar el comando
        perror("No ejecutado");
        exit(EXIT_FAILURE); //sale si no se ha podido ejecutar
    } else {
        if (!segundoPlano) { //si no es segundo plano
            int status;
            waitpid(pid, &status, 0);  //espera a que el proceso hijo termine
        } else {
            tablaProcesos[*numeroJobs].time = time(NULL); //almacena valores en la tabla de procesos
            tablaProcesos[*numeroJobs].pid = pid;
            tablaProcesos[*numeroJobs].uid = getuid();
            tablaProcesos[*numeroJobs].out = 0;
            strncpy(tablaProcesos[*numeroJobs].state, "ACTIVO", sizeof(tablaProcesos[*numeroJobs].state) - 1);
            char concatenatedArgs[MAX_INPUT_LENGTH_2] = "";
            for (int i = 0; i < 5 && tr[i] != NULL; ++i) {
                strcat(concatenatedArgs, tr[i]);
                strcat(concatenatedArgs, " ");
            }
            // Almacenar la cadena concatenada en tablaProcesos[*numeroJobs].process
            strncpy(tablaProcesos[*numeroJobs].process, concatenatedArgs,
                    sizeof(tablaProcesos[*numeroJobs].process) - 1);
            (*numeroJobs)++; //aumenta el numero de procesos
            if (tr[1] != NULL) return;
        }
    }
}





/*ejecuta un programa sin crear un proceso*/
void exec(char *tr[], int i) {
    if (tr[1] == NULL) return;

    int contadorTR = i;

    if (strcmp(tr[i - 1], "@pri") == 0 && tr[i] == NULL) { //si tr[i-1] es @pri y tr[i] es nulo
        contadorTR -= 1;
    }

    char *args[30];
    int indiceARGS = 0;
    for (int j = 1; j < contadorTR; j++) {  //recorre los argumentos
        if (tr[j] != NULL) {  //si el argumento no es nulo
            args[indiceARGS++] = tr[j]; //almacena el argumento
        }
    }
    args[indiceARGS] = NULL; //el ultimo argumento es nulo

    execvp(tr[1], args); //ejecuta el comando

    perror("execvp");
}

void Cmd_fork(char *tr[], int *numeroJobs) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        *numeroJobs = 0;
        printf("ejecutando proceso %d\n", getpid());
    } else if (pid != -1)
        waitpid(pid, NULL, 0);
}

/*comprueba si cambio señal*/
void comprobarCambioSenal(struct jobsab *job, int *status) {
    if (WIFEXITED(*status)) { //si el proceso ha terminado
        strcpy(job->state, "FINISHED");
        job->out = WEXITSTATUS(*status); //obtiene el valor de salida
    } else if (WIFSIGNALED(*status)) { //si el proceso ha terminado por una señal
        strcpy(job->state, "SIGNALED");
        job->out = WTERMSIG(*status);
    } else if (WIFSTOPPED(*status)) { //si el proceso ha sido parado
        strcpy(job->state, "STOPPED");
        job->out = WSTOPSIG(*status);
    } else if (WIFCONTINUED(*status)) { //si el proceso ha sido continuado
        strcpy(job->state, "ACTIVE");
        job->out = 0;
    }
}

/*muestra procesos*/
int jobs(struct jobsab tablaJobs[], int *numeroJobs) {
    if (*numeroJobs == 0) return 0;
    for (int i = 0; i < *numeroJobs; ++i) { //recorre los procesos
        int status;
        if (waitpid(tablaJobs[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED) >
            0) { //comprueba si ha cambiado la señal
            comprobarCambioSenal(&tablaJobs[i], &status);
        }
        time_t tiempo = tablaJobs[i].time; //obtiene el tiempo
        struct tm *fechaSI; //
        fechaSI = localtime(&tiempo);
        int dia = fechaSI->tm_mday, mes = fechaSI->tm_mon + 1, ano =
                fechaSI->tm_year + 1900, hora = fechaSI->tm_hour, min = fechaSI->tm_min, seg = fechaSI->tm_sec;
        struct passwd *p; //obtiene informacion del usuario
        if ((p = getpwuid(tablaJobs[i].uid)) == NULL) { strcpy((char *) p, " ??????"); } //si no se ha podido obtener
        printf("\t%d %12s p=%d %02d/%02d/%04d %02d:%02d:%02d %s (%03d) %s\n", tablaJobs[i].pid,
               p->pw_name, //muestra informacion
               getpriority(PRIO_PROCESS, tablaJobs[i].pid), dia, mes, ano, hora, min, seg, tablaJobs[i].state,
               tablaJobs[i].out, tablaJobs[i].process);
    }
    return 0;
}

/*pasa un proceso a primer plano o muestra infromacion de un proceso*/
int job(char *tr[], struct jobsab tablaJobs[], int *numeroJobs) {
    if (*numeroJobs == 0) return 0;
    else if (tr[1] != NULL) { //si tr[1] no es nulo
        if (strcmp(tr[1], "-fg") == 0) { //si tr[1] es -fg
            if (tr[2] == NULL) return 0;
            int pid = atoi(tr[2]); //pasamos tr[2] a entero
            int status, procesoEncontrado = 0;
            for (int i = 0; i < *numeroJobs; ++i) { //recorre los procesos
                if (tablaJobs[i].pid == pid) { //si el proceso coincide con el pid
                    procesoEncontrado = 1;
                    waitpid(pid, &status, 0); //espera a que el proceso termine
                    for (int j = i; j < *numeroJobs - 1; ++j) { //elimina el proceso de la tabla de procesos
                        tablaJobs[j] = tablaJobs[j + 1];
                    }
                    (*numeroJobs)--;
                    break;
                }
            }
            if (!procesoEncontrado) { //si no se ha encontrado el proceso
                printf("Proceso no encontrado\n");
            }
        } else if (tr[2] == NULL) { //si tr[2] es nulo
            for (int i = 0; i < *numeroJobs; ++i) {
                if (tablaJobs[i].pid == atoi(tr[1])) { //si el proceso coincide con el pid
                    int status;
                    if (waitpid(tablaJobs[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) { //comprueba si ha cambiado la señal
                        comprobarCambioSenal(&tablaJobs[i], &status);
                    }
                    struct passwd *p; //obtiene informacion del usuario
                    if ((p = getpwuid(tablaJobs[i].uid)) == NULL) { //si no se ha podido obtener
                        strcpy((char *) p, " ??????");
                    }
                    time_t tiempo = tablaJobs[i].time;
                    struct tm *fechaSI;
                    fechaSI = localtime(&tiempo);
                    int dia = fechaSI->tm_mday, mes = fechaSI->tm_mon + 1, ano =
                            fechaSI->tm_year +
                            1900, hora = fechaSI->tm_hour, min = fechaSI->tm_min, seg = fechaSI->tm_sec;
                    printf("\t%d %12s p=%d %02d/%02d/%04d %02d:%02d:%02d %s (%03d) %s\n", tablaJobs[i].pid, //muestra informacion
                           p->pw_name,
                           getpriority(PRIO_PROCESS, tablaJobs[i].pid), dia, mes, ano, hora, min, seg,
                           tablaJobs[i].state, tablaJobs[i].out, tablaJobs[i].process);
                    break;
                }
            }
        }
    } else {
        jobs(tablaJobs, numeroJobs); //muestra los procesos
    }
    return 0;
}

/*elimina procesos*/
void deljobs(char *tr[], struct jobsab tablaJobs[], int *numeroJobs) {
    if (tr[1] == NULL) return;
    else if (strcmp(tr[1], "-term") == 0) { //si tr[1] es -term
        int i = 0;
        while (i < *numeroJobs) {
            int status;
            if (waitpid(tablaJobs[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) { //comprueba si ha cambiado la señal
                comprobarCambioSenal(&tablaJobs[i], &status);
            }
            if (strcmp(tablaJobs[i].state, "FINISHED") == 0) { //si estado es TERMINADO
                for (int j = i; j < *numeroJobs - 1; ++j) { //elimina el proceso de la tabla de procesos
                    tablaJobs[j] = tablaJobs[j + 1];
                }
                (*numeroJobs)--;
            } else {
                i++;
            }
        }
    } else if (strcmp(tr[1], "-sig") == 0) { //si tr[1] es -sig
        int i = 0;
        while (i < *numeroJobs) {
            int status;
            if (waitpid(tablaJobs[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) {
                comprobarCambioSenal(&tablaJobs[i], &status);
            }
            if (strcmp(tablaJobs[i].state, "SIGNALED") == 0) {
                for (int j = i; j < *numeroJobs - 1; ++j) {
                    tablaJobs[j] = tablaJobs[j + 1];
                }
                (*numeroJobs)--;
            } else {
                i++;
            }
        }
    }
}