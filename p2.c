/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/


#include "firstshellfunctions.h"
#include "list.h"
#include "filefunctions.h"
#include "memfunctions.h"
#include "processesfunctions.h"

#define MAX_INPUT_LENGTH 200


int TrocearCadena(char *cadena, char *trozos[]) {
    int i = 1;
    if ((trozos[0] = strtok(cadena, " \n\t")) == NULL)
        return 0;
    while (i < 30) {
        if ((trozos[i] = strtok(NULL, " \n\t")) == NULL) {
            for (int j = i; j < 30; j++) {
                trozos[j] = NULL;
            }
            break;
        }
        i++;
    }
    return i;
}

void printPrompt() {
    printf("->");
}


int leerEntrada(char nE[], char *tr[], tList *L) {
    fgets(nE, MAX_INPUT_LENGTH, stdin);
    insertItem(L, nE);
    int i = TrocearCadena(nE, tr);
    return i;
}

// Definir el tipo de la tablaBloq y numBloques antes de la función si no se han declarado

void procesarEntrada(char *tr[], int i, tList *L, bool *fin, struct fichab tablaFicheros[],
                struct dirab tablaDirectorios[], struct bloqab tablaBloques[], struct jobsab tablaProcesos[], int *numOpenCommands,
                int *numeroFicheros, int *numeroDirectorios, int *numeroBloques, int *numeroJobs,
                DirParams *params) {
    if (i > 0) {
        if (strcmp(tr[0], "authors") == 0) authors(tr);
        else if (strcmp(tr[0], "getpid") == 0) pid(tr);
        else if (strcmp(tr[0], "date") == 0 || strcmp(tr[0], "hour") == 0) date(tr);
        else if (strcmp(tr[0], "chdir") == 0 || strcmp(tr[0], "getcwd") == 0) chdirr(tr);
        else if (strcmp(tr[0], "infosys") == 0 || strcmp(tr[0], "uname") == 0) infosys(tr);
        else if (strcmp(tr[0], "help") == 0) help(tr);
        else if (strcmp(tr[0], "historic") == 0) hist(tr, L);
        else if (strcmp(tr[0], "comand") == 0) {
            //Necesario que esté aquí por la recursividad
            if (tr[1] == NULL) return;
            int numero = (int) strtol(tr[1], NULL, 10);
            int n = lengL(*L);
            if (n == 1 || numero < 1 || n == numero) {
                perror("Comando no existente");
                return;
            }
            tPoss p = L->start;
            for (int f = 1; f < numero; ++f) {
                p = p->next;
                if (p == NULL) {
                    perror("Comando no existente");
                    return;
                }
            }
            tItemL rc;
            strcpy(rc, p->it);
            TrocearCadena(rc, tr);
            if (tr[1] != NULL) {
                if ((int) strtol(tr[1], NULL, 10) == numero) {
                    perror("Comando no posible de ejecutar");
                    return;
                }
            }
            procesarEntrada(tr, i, L, fin, tablaFicheros, tablaDirectorios, tablaBloques, tablaProcesos,
                            numOpenCommands,
                            numeroFicheros, numeroDirectorios, numeroBloques, numeroJobs, params);

        } else if (strcmp(tr[0], "open") == 0) Cmd_open(tr, numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "close") == 0) Cmd_close(tr, numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "dup") == 0) Cmd_dup(tr, numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "listopen") == 0) listOpen(*numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "create") == 0) create(tr, numeroDirectorios, tablaDirectorios);
        else if (strcmp(tr[0], "delrec") == 0) delrec(tr);
        else if (strcmp(tr[0], "erase") == 0) erase(tr);
        else if (strcmp(tr[0], "writestr") == 0) writestr(tr, numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "lseek") == 0) seek(tr, numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "setdirparams") == 0) setdirparams(tr, params);
        else if (strcmp(tr[0], "getdirparams") == 0) getdirparams(params);
        else if (strcmp(tr[0], "dir") == 0) dirComando(tr, params);
        else if (strcmp(tr[0], "malloc") == 0) mallocc(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "readfile") == 0) readd(tr);
        else if (strcmp(tr[0], "free") == 0) freeAddr(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "writefile") == 0) writee(tr);
        else if (strcmp(tr[0], "read") == 0) readfd(tr, *numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "write") == 0) writefd(tr, *numeroFicheros, tablaFicheros);
        else if (strcmp(tr[0], "mmap") == 0) CmdMmap(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "memdump") == 0) memdump(tr);
        else if (strcmp(tr[0], "recurse") == 0)
            if (tr[1] == NULL) return;
            else recurse(atoi(tr[1]));

        else if (strcmp(tr[0], "shared") == 0) SharedCreate(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "memfill") == 0) memfill(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "mem") == 0) mem(tr, numeroBloques, tablaBloques);
        else if (strcmp(tr[0], "uid") == 0) uid(tr);
        else if (strcmp(tr[0], "job") == 0) job(tr, tablaProcesos, numeroJobs);
        else if (strcmp(tr[0], "jobs") == 0) jobs(tablaProcesos, numeroJobs);
        else if (strcmp(tr[0], "envvar") == 0) {
            extern char **environ;
            envvar(tr, environ);
        }
        else if (strcmp(tr[0], "showenv") == 0) {
            extern char **environ;
            showenv(tr, environ);
        } else if (strcmp(tr[0], "exec") == 0) exec(tr, i);
        else if (strcmp(tr[0], "fork") == 0) Cmd_fork(tr, numeroJobs);
        else if (strcmp(tr[0], "deljobs") == 0) deljobs(tr, tablaProcesos, numeroJobs);


        else if (strcmp(tr[0], "exit") == 0 || strcmp(tr[0], "quit") == 0 || strcmp(tr[0], "bye") == 0) {
            liberarMemoria(*numeroBloques, tablaBloques);
            free_env_allocated();
            *fin = true;
        } else if (strcmp(tr[i - 1], "&") == 0) {
            tr[i - 1] = NULL;
            comandosExternos(tr, true, tablaProcesos, numeroJobs);
        } else {
            comandosExternos(tr, false, tablaProcesos, numeroJobs);
        }
    }
}


int main() {
    bool fin = false;
    char *tr[30];

    DirParams parametrosDir = { false, false, false, 0 }; // long=false, link=false, hid=false, norec

    struct fichab tablaFicheros[MAX_FICHEROS];
    struct dirab tablaDirectorios[MAX_DIRECTORIOS];
    struct bloqab tablaBloques[MAX_BLOQUES];
    struct jobsab tablaProcesos[MAX_PROCESOS];


    int numOpenCommands = 0;
    int numeroFicheros = 0;
    int numeroDirectorios = 0;
    int numeroBloques = 0;
    int numeroJobs = 0;
    int i;

    char nE[MAX_INPUT_LENGTH];
    tList L;
    createList(&L);
    parentProcess(&numOpenCommands, &numeroFicheros, tablaFicheros);
    while (!fin) {
        printPrompt();
        i = leerEntrada(nE, tr, &L);
        procesarEntrada(tr, i, &L, &fin, tablaFicheros,tablaDirectorios, tablaBloques, tablaProcesos,&numOpenCommands, &numeroFicheros,
            &numeroDirectorios, &numeroBloques, &numeroJobs, &parametrosDir);
    }

    clearList(&L);
    return 0;
}
