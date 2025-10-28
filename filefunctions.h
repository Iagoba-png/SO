/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/


#ifndef SO2025_2026_FILEFUNCTIONS_H
#define SO2025_2026_FILEFUNCTIONS_H


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>

#define MAX_DIRECTORIOS 100

struct dirab {
    char nombre[200];
};

typedef struct {
    bool longFormat; // long | short
    bool showLinks; // link | nolink
    bool showHidden; // hid | nohid
    int recursion; // 0 = norec, 1 = reca, 2 = recb
} DirParams;

void obtenerDirectorioActual();

void create(char *tr[], int *numerodirectorios, struct dirab tabladirectorios[]);

bool esDirectorioVacio(const char *path);

void erase(char *tr[]);

void delrec(char *tr[]);

void setdirparams(char *tr[], DirParams *params);
void getdirparams(DirParams *params);
void dirComando(char *tr[], DirParams *params);



#endif //SO2025_2026_FILEFUNCTIONS_H