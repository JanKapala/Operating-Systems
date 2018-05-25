#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define _XOPEN_SOURCE

pid_t child_pid;
int signalsReceivedByChild = 0;
int signalsReceivedByParent = 0;
int signalsSentByParent = 0;

// Parent handlers
void sigusr1ParentHandler(int signum)
{
    signalsReceivedByParent++;
    // printf("Parent got %dth feedback SIGUSR1 signal\n", signalsReceivedByParent);
}

void sigusr2ParentHandler(int signum)
{
    //Nothing yet (and maybe forever :O )
}

void  sigintParentHandler(int signum)
{
    kill(child_pid, SIGUSR2);
    exit(0);
}

// Child Handlers
void sigusr1ChildHandler(int signum)
{
    signalsReceivedByChild++;
    // printf("Child got %dth SIGUSR1 signal\n", signalsReceivedByChild);
}

void sigusr2ChildHandler(int signum)
{
    // printf("Child got signal SIGUSR2 so it exited\n");
    exit(0);
}

int main(int argc, char** argv)
{
    // INPUT CONTROL SECTION
    // Number of arguments checking
    if(argc != 3)
    {
        printf("Invalid number of arguments, provided: %d, should be: 2\n", argc-1);
        exit(0);
    }

    // Arguments' converting
    int L = strtol(argv[1],(char **)NULL, 10);
    int Type = strtol(argv[2],(char **)NULL, 10);

    // Arguments' values checking
    if(L < 0)
    {
        printf("Invalid first argument, it should be greater or equal zero\n");
        exit(0);
    }
    if(Type < 1 || Type >3)
    {
        printf("Invalid second argument, it should be natural number in range of: [1;3]\n");
        exit(0);
    }
    if(Type == 3)
    {
        printf("Option 'Type=3' (second argument) is not available yet. Comming soon ;)\n");
        exit(0);
    }

    /*




    */

    // ALGORITHM SECTION
    // For parent
    sigset_t emptyMask;
    sigemptyset(&emptyMask);

    // For child
    sigset_t waitForUSRsignals;
    sigfillset(&waitForUSRsignals);
    sigdelset(&waitForUSRsignals, SIGUSR1);
    sigdelset(&waitForUSRsignals, SIGUSR1);

    // For both
    sigset_t blockAll;
    sigfillset(&blockAll);

    // Setting blockAll
    if(sigprocmask(SIG_SETMASK, &blockAll, NULL) < 0)
    {
        printf("Error: couldn't block all signals\n");
    }
    // else printf("All signals are being blocked\n");

    child_pid = fork();
    if(child_pid==0)
    {
        // Child process
        // Critical signal-free code
        signal(SIGUSR1, sigusr1ChildHandler);
        signal(SIGUSR2, sigusr2ChildHandler);
        
        if(Type==1)
        {
            if(sigprocmask(SIG_SETMASK, &waitForUSRsignals, NULL) < 0)
            {
                printf("Error: couldn't set mask 'waitForUSRsignalst' at the child\n");
            }
            // else printf("Mask 'waitForUSRsignals' is set at the child\n");

            // Receiving signals from the parent
            while(signalsReceivedByChild<L) sigsuspend(&waitForUSRsignals);

            // Sending feedback signals to the parent
            for(int i=0;i<L;i++)
            {
                kill(getppid(), SIGUSR1);
                sleep(1);//It crashes without it, yeah, I know that it's intentional
            }
        }
        else if(Type==2)
        {
            // Receiving and sending signals
            for(int i=0;i<L;i++)
            {
                sigsuspend(&waitForUSRsignals);
                kill(getppid(), SIGUSR1);
            }
        }
        // Summary
        printf("Amount of signals received by Child: %d\n", signalsReceivedByChild);
    }
    else if(child_pid < 0)
    {
        //Error handling
        printf("Error: Child couldn't be created. Exiting.\n");
        exit(0);
    }
    else
    {
        //Parent process
        // Critical signal-free code
        signal(SIGUSR1, sigusr1ParentHandler);
        signal(SIGUSR2, sigusr2ParentHandler);
        signal(SIGINT, sigintParentHandler);

        if(Type==1)
        {
            if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) < 0)
            {
                printf("Error: couldn't set mask 'emptyMask' at the parent\n");
            }
            // else printf("Mask 'emptyMask' is set at the parent\n");

            // A small bouquet of information
            printf("Don't worry, it is working\n");
            printf("There is a 'sleep(1)' in loops which sends and receives signals so...\n");
            printf("It will take L seconds\n");
            printf("Be patient\n");
            printf("If you don't want to be patient you can run this program with Type (second argument) equal 2\n");

            // Sending signals to the child
            for(int i=0;i<L;i++)
            {
                kill(child_pid, SIGUSR1);
                signalsSentByParent++;
                sleep(1);//It crashes without it, yeah, I know that it's intentional
            }
            // printf("All USR1 signals have been sent to child");

            // Receiving feedback signals from the child
            while(signalsReceivedByParent<L) sigsuspend(&emptyMask);
        }
        else if(Type==2)
        {
            // Sending and receiving signals
            for(int i=0;i<L;i++)
            {
                signalsSentByParent++;
                kill(child_pid, SIGUSR1);
                sigsuspend(&emptyMask);
            } 
        }
        kill(child_pid, SIGUSR2);

        // Summary
        printf("Amount of signals sent to child: %d\n", signalsSentByParent);
        printf("Amount of signals received from child: %d\n", signalsReceivedByParent);
    }

    exit(0);
}