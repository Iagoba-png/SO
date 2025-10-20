/*
AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/


#ifndef UNTITLED_FIRSTSHELLFUNCTIONS_H
#define UNTITLED_FIRSTSHELLFUNCTIONS_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include "list.h"


#define MAX_COMANDOS 39
#define MAX_FICHEROS 100

struct fichab {
    int descrip;
    char nombre[200];
    int modo;
};

struct comando {
    char nombreComando[15];
    char explicacionComando[250];
};

void authors(char *tr[]);

void pid(char *tr[]);

void date(char *tr[]);

void timee(char *tr[]);

void chdirr(char *tr[]);

void infosys(char *tr[]);

void help(char *tr[]);

void hist(char *tr[], tList *L);

void Cmd_open(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]);

void Cmd_close(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]);

void Cmd_dup(char *tr[], int *numeroFicheros, struct fichab tablaFicheros[]);

void listOpen(int numeroFicheros, struct fichab tablaFicheros[]);

void parentProcess(int *numOpenCommands, int *numeroFicheros, struct fichab tablaFicheros[]);

#endif //UNTITLED_FIRSTSHELLFUNCTIONS_H
