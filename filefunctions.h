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

#define MAX_DIRECTORIOS 100

struct dirab {
    char nombre[200];
};

void obtenerDirectorioActual();

void create(char *tr[], int *numerodirectorios, struct dirab tabladirectorios[]);

bool esDirectorioVacio(const char *path);

void erase(char *tr[]);

void delrec(char *tr[]);

#endif //SO2025_2026_FILEFUNCTIONS_H