#include "confirmations.h"
void waitForConfirmation(BarberSalon* barberSalon, long int semID)
{
    takeSemaphore(semID);
    barberSalon->confirmation=1;
    giveSemaphore(semID);
    while(1)
    {
        takeSemaphore(semID);
        if(barberSalon->confirmation==0)
        {
            giveSemaphore(semID);
            break;
        }
        giveSemaphore(semID);
    }
}

void confirm(BarberSalon* barberSalon, long int semID)
{
    while(1)
    {
        takeSemaphore(semID);
        if(barberSalon->confirmation==1)
        {
            // If you want to turn on interactive mode uncomment below functions
            // printf("Press any key to confirm\n");
            // getchar();
            barberSalon->confirmation=0;
            giveSemaphore(semID);
            break;
        }
        giveSemaphore(semID);
    }
}