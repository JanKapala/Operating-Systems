#include "semLib.h"

long int createAndTakeSemaphore(char key)
{
    // Semaphore making
    long int semID = semget(getKey(key), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    if(semID==-1) {printf("Error on semget during making: %s\n", strerror(errno));exit(0);}

    // Semaphore initialization
    union semun setter;
    setter.val=1;
    int ret = semctl(semID, 0, SETVAL, setter);
    if(ret == -1) {printf("Error on semctl during initialization: %s\n", strerror(errno));exit(0);}

    takeSemaphore(semID);
    return semID;
}

long int getExistingSemaphore(char key)
{
    return semget(getKey(key), 0, S_IWUSR | S_IRUSR);
}

void takeSemaphore(long int semID)
{
    // Take semaphore
    struct sembuf operation;
    operation.sem_num=0;
    operation.sem_op=-1;
    operation.sem_flg=0;
    semop(semID, &operation, 1);
}

void giveSemaphore(long int semID)
{
    // Give semaphore;
    struct sembuf operation;
    operation.sem_num=0;
    operation.sem_op=1;
    operation.sem_flg=0;
    semop(semID, &operation, 1);
}

void deleteSemaphore(long int semID)
{
    int ret = semctl(semID, 0, IPC_RMID, NULL);
    if(ret == -1) {printf("Error during deleting semaphore %ld: %s\n", semID, strerror(errno));exit(0);}
}