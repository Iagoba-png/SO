//
// Created by usuario on 29/11/25.
//

#ifndef P2_PROCESSESFUNCTIONS_H
#define P2_PROCESSESFUNCTIONS_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <sys/resource.h>


#define MAXVAR 100
#define MAX_PROCESOS 100
#define MAX_INPUT_LENGTH_2 200




struct jobsab{
    int pid;
    int uid;
    char process[20];
    char state[1024];
    time_t time;
    int out;
};


void uid(char *tr[]);

void envvar(char *tr[], char *envp[]);

void free_env_allocated(void);

void showenv(char *tr[],char *e[]);

void comandosExternos(char *tr[], bool segundoPlano, struct jobsab tablaProcesos[], int *numeroJobs);

void exec(char *args[], int i);

int job(char *tr[],struct jobsab tablaJobs[], int *numeroJobs);

int jobs(struct jobsab tablaJobs[], int *numeroJobs);

void deljobs(char *tr[],struct jobsab tablaJobs[], int *numeroJobs);

void Cmd_fork(char *tr[], int *numeroJobs);


























#endif //P2_PROCESSESFUNCTIONS_H