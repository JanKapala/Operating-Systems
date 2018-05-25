#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>


#define QUEUE_MAX_SIZE 1024

#define AWAKE 0
#define SLEEPING 1


#define SALON_KEY 10
#define SEM1_KEY 11

long int getKey(char projectID);
void getTimestamp(char* timestamp);

#endif