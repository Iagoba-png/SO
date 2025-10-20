/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/


#ifndef P0BIEN_LIST_H
#define P0BIEN_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef char tItemL[200];

typedef struct Node *tPoss;

struct Node {
    tItemL it;
    tPoss next;
};

struct List {
    tPoss start;
    tPoss end;
};

typedef struct List tList;

void createList(tList *L);

void insertItem(tList *L, tItemL nit);

void clearList(tList *L);

int lengL(tList L);

#endif //P0BIEN_LIST_H
