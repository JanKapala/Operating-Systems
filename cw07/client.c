#include "semLib.h"
#include "salonLib.h"
#include "confirmations.h"

int main(int argc, char** argv)
{
    if(argc!=3)
    {
        printf("Invalid number of arguments. Exiting.\n");
        exit(0);
    }
    int N = strtoul(argv[1], NULL, 10); // Number of clients
    int S = strtoul(argv[2], NULL, 10); // Number of how many times client should go to the barber
    if(N < 1)
    {
        printf("Invalid number of clients (first argument). Exiting.\n");
        exit(0);
    }
    if(S < 1)
    {
        printf("Invalid number of how many times client should go to the barber (second argument). Exiting.\n");
        exit(0);
    }
    
    char timestamp[64];

    int pid;
    for(int i=0;i<N;i++)
    {
        pid = fork();
        if(pid==0)
        {
            // Semaphore obtaining
            long int sem1ID = -1;
            while((sem1ID=getExistingSemaphore(SEM1_KEY))==-1){printf("Waiting for semaphore ID "); sleep(1);};
            printf("Semaphore ID obtained\n");

            // salon obtaining
            takeSemaphore(sem1ID);
            BarberSalon* barberSalon = getExistingBarberSalon(SALON_KEY);
            giveSemaphore(sem1ID);

            for(int j=0;j<S;j++)
            {
                takeSemaphore(sem1ID);
                if(isQueueFull(barberSalon))
                {
                    getTimestamp(timestamp);
                    printf("There is no place in the queue, I am leaving (%s) (PID: %d)\n", timestamp, getpid());
                    giveSemaphore(sem1ID);
                    continue;
                }
                
                push(barberSalon, getpid());
                
                if(isSleeping(barberSalon))
                {
                    awake(barberSalon);
                    getTimestamp(timestamp);
                    printf("I have just woken barber up                  (%s) (PID: %d)\n", timestamp, getpid());
                }
                giveSemaphore(sem1ID);

                // Confirm invitation if it is right;
                while(1)
                {
                    takeSemaphore(sem1ID);
                    if(barberSalon->curClientID==getpid())
                    {
                        giveSemaphore(sem1ID);
                        break;
                    }
                    giveSemaphore(sem1ID);
                }
                confirm(barberSalon, sem1ID);

                // Take place
                getTimestamp(timestamp);
                printf("I have taken place                           (%s) (PID: %d)\n", timestamp, getpid());
                waitForConfirmation(barberSalon, sem1ID);

                // Wait for cutting end
                waitForConfirmation(barberSalon, sem1ID);
                getTimestamp(timestamp);
                printf("Cutting done, I am leaving                   (%s) (PID: %d)\n\n", timestamp, getpid());

                giveSemaphore(sem1ID);
            }
            exit(0);
        }
    }
    exit(0);
}