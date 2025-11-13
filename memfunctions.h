/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba  iago.alcoba@udc.es
    GRUPO: 1.3
*/


#ifndef P2_MEMFUNCTIONS_H
#define P2_MEMFUNCTIONS_H



#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/shm.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>


#define TAMANO 2048
#define MAX_BLOQUES 100

#define OPCION_MALLOC 1
#define OPCION_MMAP 2
#define OPCION_SHARED 3

struct bloqab {
    char nombreArchivo[100];
    int descriptor;
    void *direccionMemoria;
    time_t tiempo;
    ssize_t tamano;
    char modoMemoria[7];
    key_t clave;
};

void liberarMemoria(int numeroBloques, struct bloqab tablaBloques[]);

void mallocc(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]);

void CmdMmap(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]);

void readd(char *tr[]);

void writee(char *tr[]);

void memdump(char *tr[]);

void recurse(int n);

void SharedCreate(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]);

void memfill(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]);

void mem(char *tr[], int *numeroBloques, struct bloqab tablaBloques[]);



#endif //P2_MEMFUNCTIONS_H