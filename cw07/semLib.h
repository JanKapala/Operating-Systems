#ifndef SEMLIB_H
#define SEMLIB_H

#include "common.h"

union semun
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

long int createAndTakeSemaphore(char key);

long int getExistingSemaphore(char key);

void takeSemaphore(long int semID);

void giveSemaphore(long int semID);

void deleteSemaphore(long int semID);

#endif