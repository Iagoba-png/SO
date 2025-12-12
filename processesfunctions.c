//
// Created by usuario on 29/11/25.
//

#include "processesfunctions.h"



/*muestra o cambia credenciales  PracticaSO3*/
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

int BuscarVariable(char *var, char *e[]) {
    int pos = 0;
    char aux[MAXVAR];

    strcpy(aux, var);
    strcat(aux, "=");

    while (e[pos] != NULL) {
        if (!strncmp(e[pos], aux, strlen(aux)))
            return pos;
        pos++;
    }
    errno = ENOENT;
    return -1;
}

int CambiarVariable(char *var, char *valor, char *e[]) {
    int pos;
    char *aux;

    if ((pos = BuscarVariable(var, e)) == -1)
        return -1;

    aux = malloc(strlen(var) + strlen(valor) + 2);
    if (aux == NULL)
        return -1;

    sprintf(aux, "%s=%s", var, valor);
    e[pos] = aux;
    return pos;
}



void envvar_show(char *var, char *envp[]) {
    extern char **environ;

    int pos1 = BuscarVariable(var, envp);
    int pos2 = BuscarVariable(var, environ);
    char *val = getenv(var);

    if (pos1 == -1) {
        printf("Variable %s no encontrada\n", var);
        return;
    }

    printf("Acceso por envp     : %s (%p) @%p\n",
           envp[pos1], envp[pos1], &envp[pos1]);

    printf("Acceso por environ  : %s (%p) @%p\n",
           environ[pos2], environ[pos2], &environ[pos2]);

    printf("Acceso por getenv   : %s (%p)\n",
           val, val);
}


#define MAX_ENV_ALLOC 100

static char *env_allocated[MAX_ENV_ALLOC];
static int env_count = 0;


void envvar_change(char *mode, char *var, char *value, char *envp[]) {
    extern char **environ;

    if (strcmp(mode, "-a") == 0) {             // acceso por envp[]
        int pos = BuscarVariable(var, envp);
        if (pos == -1) {
            printf("Variable %s no encontrada\n", var);
            return;
        }
        if (CambiarVariable(var, value, envp) == -1)
            perror("Error al cambiar variable");

    } else if (strcmp(mode, "-e") == 0) {      // acceso por environ
        int pos = BuscarVariable(var, environ);
        if (pos == -1) {
            printf("Variable %s no encontrada\n", var);
            return;
        }
        if (setenv(var, value, 1) != 0)
            perror("Error con setenv");

    } else if (strcmp(mode, "-p") == 0) {      // putenv (crea si no existe)
        char *new = malloc(strlen(var) + strlen(value) + 2);
        sprintf(new, "%s=%s", var, value);
        if (putenv(new) != 0) {
            perror("putenv");
            free(new);
            return;
        }
        env_allocated[env_count++] = new;

    } else {
        printf("Uso: envvar -change [-a|-e|-p] var val\n");
    }


}

void free_env_allocated(void) {
    for (int i = 0; i < env_count; i++) {
        free(env_allocated[i]);
    }
    env_count = 0;
}




void envvar_subs(char *mode, char *var1, char *var2, char *value, char *envp[]) {
    extern char **environ;
    int pos_old, pos_new;

    if (strcmp(mode, "-a") == 0) {
        pos_old = BuscarVariable(var1, envp);
        pos_new = BuscarVariable(var2, envp);

        if (pos_old == -1) {
            printf("Variable %s no existe\n", var1);
            return;
        }
        if (pos_new != -1) {
            printf("Variable %s ya existe\n", var2);
            return;
        }

        char *new = malloc(strlen(var2) + strlen(value) + 2);
        sprintf(new, "%s=%s", var2, value);
        envp[pos_old] = new;

    } else if (strcmp(mode, "-e") == 0) {
        pos_old = BuscarVariable(var1, environ);
        pos_new = BuscarVariable(var2, environ);

        if (pos_old == -1) {
            printf("Variable %s no existe\n", var1);
            return;
        }
        if (pos_new != -1) {
            printf("Variable %s ya existe\n", var2);
            return;
        }

        char *new = malloc(strlen(var2) + strlen(value) + 2);
        sprintf(new, "%s=%s", var2, value);
        environ[pos_old] = new;

    } else {
        printf("Uso: envvar -subs [-a|-e] var1 var2 valor\n");
    }
}


void envvar(char *tr[], char *envp[]) {
    if (tr[1] == NULL) {
        printf("Uso: envvar [-show|-change|-subs] args...\n");
        return;
    }

    if (strcmp(tr[1], "-show") == 0 && tr[2] != NULL) {
        envvar_show(tr[2], envp);
        return;
    }

    if (strcmp(tr[1], "-change") == 0 && tr[2] && tr[3] && tr[4]) {
        envvar_change(tr[2], tr[3], tr[4], envp);
        return;
    }

    if (strcmp(tr[1], "-subs") == 0 && tr[2] && tr[3] && tr[4] && tr[5]) {
        envvar_subs(tr[2], tr[3], tr[4], tr[5], envp);
        return;
    }

    printf("Uso incorrecto del comando envvar\n");
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
/* ejecuta comandos externos según PROGSPEC */
void comandosExternos(char *tr[], bool segundoPlano,
                      struct jobsab tablaProcesos[], int *numeroJobs)
{
    int prioridad = -1;
    int nargs = 0;

    /* 1. Contar argumentos reales */
    while (tr[nargs] != NULL)
        nargs++;

    /* 2. Detectar si el último token es "&" */
    if (nargs > 0 && strcmp(tr[nargs - 1], "&") == 0) {
        segundoPlano = true;
        tr[nargs - 1] = NULL;     // eliminar &
        nargs--;
    }

    /* 3. Detectar si el último token es @N */
    if (nargs > 0 && tr[nargs - 1][0] == '@') {
        prioridad = atoi(tr[nargs - 1] + 1);  // convertir tras '@'
        tr[nargs - 1] = NULL;                 // eliminar @N
        nargs--;
    }

    /* 4. Crear proceso */
    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso");
        return;
    }

    /* ---------- HIJO ---------- */
    if (pid == 0) {

        /* Aplicar prioridad si se especificó */
        if (prioridad != -1) {
            if (setpriority(PRIO_PROCESS, 0, prioridad) == -1)
                perror("Error al cambiar prioridad");
        }

        execvp(tr[0], tr);
        perror("No ejecutado");
        exit(EXIT_FAILURE);
    }

    /* ---------- PADRE ---------- */

    if (!segundoPlano) {           /* FOREGROUND */
        int status;
        waitpid(pid, &status, 0);
    } else {                       /* BACKGROUND */

        /* Rellenar entrada en tablaProcesos */
        tablaProcesos[*numeroJobs].time = time(NULL);
        tablaProcesos[*numeroJobs].pid = pid;
        tablaProcesos[*numeroJobs].uid = getuid();
        tablaProcesos[*numeroJobs].out = 0;
        strncpy(tablaProcesos[*numeroJobs].state, "ACTIVE",
                sizeof(tablaProcesos[*numeroJobs].state) - 1);

        /* Guardar la línea de comando original */
        char concatenatedArgs[256] = "";
        for (int i = 0; tr[i] != NULL; i++) {
            strcat(concatenatedArgs, tr[i]);
            strcat(concatenatedArgs, " ");
        }

        strncpy(tablaProcesos[*numeroJobs].process,
                concatenatedArgs,
                sizeof(tablaProcesos[*numeroJobs].process) - 1);

        (*numeroJobs)++;   // aumentar lista de procesos
    }
}






/* Ejecuta un programa SIN crear proceso (exec() real) */
void exec(char *tr[], int i) {

    if (tr[1] == NULL) return;

    int prioridad = -1;

    /* contar argumentos */
    int nargs = 0;
    while (tr[nargs] != NULL)
        nargs++;

    /* ---- ELIMINAR & SI APARECE ---- */
    if (nargs > 0 && strcmp(tr[nargs - 1], "&") == 0) {
        tr[nargs - 1] = NULL;
        nargs--;
    }

    /* -detectar prioridad */
    if (nargs > 0 && tr[nargs - 1][0] == '@') {
        prioridad = atoi(tr[nargs - 1] + 1);
        tr[nargs - 1] = NULL;   // eliminar @N del array
        nargs--;
    }

    /* array argumentos */
    char *args[64];
    int j = 0;

    // mover de tr[1] en adelante
    for (int k = 1; tr[k] != NULL; k++)
        args[j++] = tr[k];

    args[j] = NULL;

    /*cambio prioridad */
    if (prioridad != -1) {
        if (setpriority(PRIO_PROCESS, 0, prioridad) == -1)
            perror("setpriority");
    }

    /* SIN FORK */
    execvp(args[0], args);

    /* Sólo llegamos aquí si falla execvp */
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