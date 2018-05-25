#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define _XOPEN_SOURCE

int child_pid;
short int startNew = 1;
short int working = 0;
void sigtstpHandler(int signum)
{
    if(working==1)
    {
        kill(child_pid, SIGTSTP);
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
        working=0;
    }
    else startNew=1;
}

void sigintHandler(int signum)
{
    printf("\nOdebrano sygnał SIGINT\n" );
    if(working) kill(child_pid, SIGINT);
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

    while(1)
    {
        if(startNew)
        {
            child_pid = fork();
            if(child_pid==0)
            {
                execl("./script", "./script", NULL);
            }
            else if(child_pid < 0)
            {
                printf("fork() error");
                kill(child_pid, SIGTSTP);
                exit(0);
            }
            startNew=0;
            working=1;
        }
    }

    return 0;
}