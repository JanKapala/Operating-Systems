#include "semLib.h"
#include "salonLib.h"
#include "confirmations.h"
#include <signal.h>

long int sem1ID;
BarberSalon* barberSalon;

void sigHandler(int signum)
{
    deleteBarberSalon(barberSalon);
    deleteSemaphore(sem1ID);
    printf("Process exited by SIGTERM\n");
    exit(0);
}


int main(int argc, char** argv)
{
    if(argc!=2)
    {
        printf("Invalid number of arguments. Exiting.\n");
        exit(0);
    }
    int N = strtoul(argv[1], NULL, 10); // Max length of the queue
    if(N < 1 || N > QUEUE_MAX_SIZE)
    {
        printf("Invalid queue size. Exiting.\n");
        exit(0);
    }

    // Signal setting
    signal(SIGTERM, sigHandler);


    // Create resources
    sem1ID = createAndTakeSemaphore(SEM1_KEY);
    barberSalon = createBarberSalon(N, SALON_KEY);
    giveSemaphore(sem1ID);

    int hasBeenAwaken = 0;
    char timestamp[64];
    while(1)
    {
        takeSemaphore(sem1ID);

        if(isSleeping(barberSalon))
        {
            giveSemaphore(sem1ID);
            continue;
        }

        if(hasBeenAwaken==0)
        {
            getTimestamp(timestamp);
            printf("I have just been woken up               (%s)\n", timestamp);
            hasBeenAwaken=1;
        }

        if(isQueueEmpty(barberSalon))
        {
            getTimestamp(timestamp);
            printf("I go to sleep                           (%s)\n\n\n", timestamp);
            goSleep(barberSalon);
            hasBeenAwaken=0;
            giveSemaphore(sem1ID);
        }
        else
        {
            // Choose client
            pid_t clientID;
            clientID = pop(barberSalon);
            barberSalon->curClientID = clientID;
            giveSemaphore(sem1ID);

            // Invite
            getTimestamp(timestamp);
            printf("I am inviting client %d to a haircut (%s)\n", clientID, timestamp);
            // wait for confirmation of invitation
            waitForConfirmation(barberSalon, sem1ID);

            // Confirm taking place
            confirm(barberSalon, sem1ID);

            // Cutting
            takeSemaphore(sem1ID);
            getTimestamp(timestamp);
            printf("I am starting cutting client: %d     (%s)\n", clientID, timestamp);
            getTimestamp(timestamp);
            printf("Cutting client %d done!              (%s)\n", clientID, timestamp);
            giveSemaphore(sem1ID);

            confirm(barberSalon, sem1ID); // confirm end of cutting
        }
        // Uncomment below functions to see queue
        // takeSemaphore(sem1ID);
        // printf("\nThe queue:\n");
        // printQueueState(barberSalon);
        // printf("\n\n");
        // giveSemaphore(sem1ID);
    }

    deleteBarberSalon(barberSalon);
    deleteSemaphore(sem1ID);
    exit(0);
}