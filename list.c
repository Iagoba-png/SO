/*
    AUTOR 1: Alejandro Suárez García  a.suarez4@udc.es
    AUTOR 2: Iago Bescansa Alcoba iago.alcoba@udc.es
    GRUPO: 1.3
*/



#include "list.h"

void createList(tList *L) {
    L->end = NULL;
    L->start = NULL;
}

void insertItem(tList *L, tItemL nit) {
    tPoss p;
    p = malloc(sizeof(*p));
    strcpy(p->it, nit);
    p->next = NULL;
    if (L->start == NULL) {
        L->start = p;
        L->end = p;
    }
    else {
        L->end->next = p;
        L->end = p;
    }
}

void clearList(tList *L) {
    if (L->start == NULL) {}
    else {
        tPoss p = L->start;
        while (L->start != NULL) {
            L->start = L->start->next;
            free(p);
            p = L->start;
        }
    }
}

int lengL(tList L) {
    tPoss p = L.start;
    int i = 0;
    if (p == NULL) { return i; }
    else {
        while (p != NULL) {
            ++i;
            p = p->next;
        }
        return i;
    }
}
