#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAX_LINE 300

// GLOBAL VARIABLES TABLE-------------------------------------------
pthread_t mainThreadID;

// Input file:
char* path;
FILE* textfile;

// nk parameter
int nk = 3;

// verbose
int verboseFlag = 0;

// Line selecting
int L;
char operator;

// Cyclic buffer:
int N;
char** buffer;
pthread_mutex_t* bufferMutexes;

// Head/Tail indexes of cyclic buffer:
int readIndex = 0;
int writeIndex = 1;
// This mutex is responsible for access to readIndex and writeIndex:
pthread_mutex_t rwmutex = PTHREAD_MUTEX_INITIALIZER;
// Condition related to the above mutex:
pthread_cond_t rwcond = PTHREAD_COND_INITIALIZER; 

// Arrays of threads:
int P;
pthread_t* producers;
int K;
pthread_t* consumers;

// Exit routine
void exitRoutine()
{
    // Threads
    for(int i=0; i<P; i++) pthread_cancel(producers[i]);
    free(producers);
    for(int i=0; i<K; i++) pthread_cancel(consumers[i]);
    free(consumers);

    // File
    fclose(textfile);

    // Buffer
    for(int i=0; i<N+2; i++) free(buffer[i]);
    free(buffer);
    free(bufferMutexes);

    exit(0);
}

// THREADS-----------------------------------------------------------

void* producer(void* _arg)
{
    char name[40];
    snprintf(name, 40, "Thread %ld (Producer):", pthread_self());
    if(verboseFlag) fprintf(stderr, "%s Starting\n", name);

    /*An idea is that we want to write to specific index and release global
    writeIndex (and rwmutex and rwcond) to let other threads write to
    other indexes in the same time. So we have to have tempWriteIndex.*/
    int tempWriteIndex;
    while(1)
    {
        pthread_mutex_lock(&rwmutex);
        /*Comment to while loop condition:
        It check for possibility of writting, so it check if writeIndex
        is right before readIndex (in clockwise order) and it wait.*/
        while((writeIndex+1)%(N+2) == readIndex)
        {
            if(verboseFlag) fprintf(stderr, "%s Waiting for the buffer releasing\n", name);
            pthread_cond_wait(&rwcond, &rwmutex);
        }

        // ASAP we do some quick operations:
        // -> catch index pointing current slot in buffer as below:
        tempWriteIndex = writeIndex;
        // -> lock on this specific slot before releasing global writeIndex
        pthread_mutex_lock(&bufferMutexes[tempWriteIndex]);
        // -> make circular incrementation of writeIndex
        writeIndex = (writeIndex+1)%(N+2);
        // -> Wake other threads up
        pthread_cond_broadcast(&rwcond);
        // -> release global writeIndex to let other threads make use of it
        pthread_mutex_unlock(&rwmutex);

        /* Now it's time for relatively long operations: reading from file
        writting to buffer: */
        char line[MAX_LINE];
        if(fgets(line, MAX_LINE, textfile)==NULL) pthread_kill(mainThreadID, SIGINT);
        if(verboseFlag) fprintf(stderr, "%s Writing line '%s' to the buffer\n", name, line);
        buffer[tempWriteIndex] = calloc(strlen(line)+1, sizeof(char));
        strcpy(buffer[tempWriteIndex], line);
        
        pthread_mutex_unlock(&bufferMutexes[tempWriteIndex]);
    }
    pthread_exit(NULL);
}

int lengthChecker(int n, int L, char operator) // Consumer auxiliary function
{
    if(operator=='<')
    {
        if(n < L) return 1;
    }
    else if(operator=='=')
    {
        if(n == L) return 1;
    }
    else if(operator=='>')
    {
        if(n > L) return 1;
    }
    return 0;
}

void* consumer(void* _arg) // analogously to the producer
{
    printf("I'm consumer\n");
    int tempReadIndex;
    int n;
    while(1)
    {
        pthread_mutex_lock(&rwmutex);

        while((readIndex+1)%(N+2) == writeIndex)
        {
            pthread_cond_wait(&rwcond, &rwmutex);
        }

        // Below is difference between consumer and producer:
        // circular incrementation is made firsty (befor tempReadIndex assignment)
        // in opposite to producer
        readIndex = (readIndex+1)%(N+2);
        tempReadIndex = readIndex;
        pthread_mutex_lock(&bufferMutexes[tempReadIndex]);
        pthread_cond_broadcast(&rwcond);
        pthread_mutex_unlock(&rwmutex);

        n=strlen(buffer[tempReadIndex]);
        if(lengthChecker(n, L, operator)) printf("%d: %s", tempReadIndex, buffer[tempReadIndex]);
        // free(buffer[tempReadIndex]);
        
 
        pthread_mutex_unlock(&bufferMutexes[tempReadIndex]);
    }
    pthread_exit(NULL);
}

// RUTINE FUNCTIONS-------------------------------------------------

// Signal handler
void sigintHandler(int signum)
{
    exitRoutine();
}

// Start routine
void startRoutine()
{
    // Signal handling
    signal(SIGINT, sigintHandler);
    mainThreadID = pthread_self();

    // Text file openning
    textfile = fopen(path, "r");
    if(textfile==NULL)
    {
        printf("Error during file %s openning. Exiting.\n", path);
        exit(1);
    }

    // Buffer creating
    buffer = calloc(N+2,sizeof(char*)); // Cyclic buffer
    /*Comment to above line:
    2 additional slots for "reading start" and "writting stop" indexes
    These particular slots:
    -> are in circulary clockwise movement in buffer during read and write
    -> cannot be occupied by any value
    -> no of them can jump over another during clockwise movement*/ 
    bufferMutexes = calloc(N+2, sizeof(pthread_mutex_t));
    for(int i=0; i<N+2; i++) pthread_mutex_init(&bufferMutexes[i], NULL);

    // Producers and consumers spawnning
    producers = calloc(P, sizeof(pthread_t));
    for(int i=0; i<P; i++) pthread_create(&producers[i], NULL, producer, NULL);

    consumers = calloc(K, sizeof(pthread_t));
    for(int i=0; i<K; i++) pthread_create(&consumers[i], NULL, consumer, NULL);
}

int main(int argc, char** argv)
{
    // ARGUMENTS PARSING--------------------------------------------
    // Number of arguments
    if(argc != 9) {printf("Invalid number of arguments\n");exit(1);}

    // Arrays of threads
    P = (int)strtol(argv[1], NULL, 10);
    if(P <= 0) {printf("Invalid value of 1. argument (Producers number should be greater than 0)\n");exit(1);}
    K = (int)strtol(argv[2], NULL, 10);
    if(K <= 0) {printf("Invalid value of 2. argument (Consumers number should be greater than 0)\n");exit(1);}

    // Cyclic buffer:
    N = (int)strtol(argv[3], NULL, 10);
    if(N <= 0) {printf("Invalid value of 3. argument (Buffer size should be greater than 0)\n");exit(1);}

    // Input file:
    path = argv[4];

    // Line selecting
    L  = (int)strtol(argv[5], NULL, 10);
    if(L < 0) {printf("Invalid value of 5. argument (Line length should be greater or equal 0)\n");exit(1);}
    operator = argv[6][0];
    if(!(operator == '<') && !(operator == '=') && !(operator == '>'))
    {
        printf("Invalid value of 6. argument (Operator should be: '<', '=' or '>')\n");
        exit(1);
    }

    // VerboseFlag
    if(!(strcmp(argv[7],"verbose")==0) && !(strcmp(argv[7],"quiet")==0))
    {
        printf("Invalid value of 7. argument (verbose flag should be: 'verbose'  or 'quiet')\n");
        exit(1);
    }
    if((strcmp(argv[7],"verbose")==0)) verboseFlag = 1;
    else if(strcmp(argv[7],"quiet")==0) verboseFlag = 0;
    else {printf("Unidentified error. Exiting."); exit(0);}

    // nk parameter
    nk = (int)strtol(argv[8], NULL, 10);
    if(nk < 0) {printf("Invalid value of 8. argument (nk should be greater than or equal 0)\n");exit(1);}

    printf("Parameters of program:\n");
    printf("Number of producers: %d\n", P);
    printf("Number of consumers: %d\n", K);
    printf("Size of buffer: %d\n", N);
    printf("Input file path: %s\n", path);
    printf("Line length: %d\n", L);
    printf("Operator: %c\n", operator);
    printf("Verbose flag: %s (%d)\n", argv[7], verboseFlag);
    printf("nk: %d\n", nk);

    // MAIN THREAD--------------------------------------------
    startRoutine();
    
    // Waiting section
    if(nk > 0)
    {
        sleep(nk);
        exitRoutine();
    }
    else pause();
}