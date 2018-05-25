#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#define _XOPEN_SOURCE

short int stop = 0;
void sigtstpHandler(int signum)
{
    if(stop==0)
    {
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
        stop=1;
    }
    else stop = 0;
}

void sigintHandler(int signum)
{
    printf("\nOdebrano sygnał SIGINT\n" );
    exit(0);
}

int main(void)
{
    signal(SIGTSTP, sigtstpHandler);

    struct sigaction act;
    act.sa_handler = sigintHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    time_t lastRawTime;
    time(&lastRawTime);
    time_t currentRawTime;
    struct tm * timeinfo;

    while(1)
    {
        if(!stop)
        {
            time(&currentRawTime);
            if(currentRawTime>lastRawTime)
            {
                lastRawTime=currentRawTime;
                timeinfo = localtime ( &currentRawTime );
                printf ( "\nCurrent local time: %d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
            }
        }
    }
    return 0;
}