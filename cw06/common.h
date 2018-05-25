#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h> // for permission constants
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

#define PROJECT_ID 'J'

#define MAX_MESSAGE_LENGTH 128

// Types of messages
#define START_CLIENT 1
#define STOP_CLIENT 2
#define MIRROR 3
#define CALC 4
#define TIME 5
#define END 6

#define MAX_CLIENTS_AMOUNT 128

typedef struct msgbuf
{
    long type;
    char content[MAX_MESSAGE_LENGTH];
} msg;

long int getServerQueueKey();
void clrstr(char* stringToClear);