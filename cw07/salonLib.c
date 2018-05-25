#include "salonLib.h"

BarberSalon* createBarberSalon(int maxQueueSize, char salonKey)
{
    long int barberSalonID = shmget(getKey(salonKey), sizeof(BarberSalon), IPC_CREAT | S_IWUSR | S_IRUSR);
    if(barberSalonID == -1) {printf("Error on shmget during barberSalon making: %s\n", strerror(errno));exit(0);}
    BarberSalon* barberSalon = shmat(barberSalonID, NULL,0);
    if(barberSalon == (void *)-1) {printf("Error on shmat during barberSalon making: %s\n", strerror(errno));exit(0);}

    barberSalon->qMaxSize=maxQueueSize;
    barberSalon->qCurSize=0;
    for(int i=0;i<barberSalon->qMaxSize;i++) barberSalon->queue[i] = -1;
    barberSalon->isBs=SLEEPING;
    barberSalon->salonID=barberSalonID;
    barberSalon->confirmation = 0;
    barberSalon->curClientID = -1;

    return barberSalon;
}

BarberSalon* getExistingBarberSalon(char salonKey)
{
    long int barberSalonID = shmget(getKey(salonKey), 0, S_IWUSR | S_IRUSR);
    if(barberSalonID == -1) {printf("Error on shmget during barberSalon getting: %s\n", strerror(errno));exit(0);}
    BarberSalon* barberSalon = shmat(barberSalonID, NULL,0);
    if(barberSalon == (void *)-1) {printf("Error on shmat during barberSalon getting: %s\n", strerror(errno));exit(0);}

    return barberSalon;
}

void discardBarberSalon(BarberSalon* barberSalon)
{

    int ret = shmdt(barberSalon);
    if(ret == -1) {printf("Error on shmdt during barberSalon detaching: %s\n", strerror(errno));exit(0);}

    barberSalon = NULL;
}

void deleteBarberSalon(BarberSalon* barberSalon)
{
    long int tempSalonID = barberSalon->salonID;

    int ret = shmdt(barberSalon);
    if(ret == -1) {printf("Error on shmdt during barberSalon detaching: %s\n", strerror(errno));exit(0);}

    ret = shmctl(tempSalonID, IPC_RMID, NULL);
    if(ret == -1) printf("Error on semctl during salon deleting: %s\n", strerror(errno));

    barberSalon = NULL;
}

void printQueueState(BarberSalon* barberSalon)
{
    printf("Queue state:\n");

    for(int i=0;i<barberSalon->qMaxSize;i++)
    {
        if(barberSalon->queue[i]==-1) printf("%d: Empty place\n", i);
        else printf("%d: Process %d\n", i, barberSalon->queue[i]);
    }
}

int push(BarberSalon* barberSalon, int newElement)
{
    if(barberSalon->qCurSize<barberSalon->qMaxSize)
    {
        barberSalon->queue[barberSalon->qCurSize]=newElement;
        barberSalon->qCurSize++;
        return 0;
    }
    return -1;
}

int pop(BarberSalon* barberSalon)
{
    if(barberSalon->qCurSize>0)
    {
        int result = barberSalon->queue[0];
        for(int i=0;i<barberSalon->qCurSize-1;i++)
        {
            barberSalon->queue[i]=barberSalon->queue[i+1];
        }
        barberSalon->queue[barberSalon->qCurSize-1]=-1;
        barberSalon->qCurSize--;
        return result;
    }
    return -1;
}

int queueSize(BarberSalon* barberSalon)
{
    return barberSalon->qCurSize;
}

int isQueueEmpty(BarberSalon* barberSalon)
{
    if(queueSize(barberSalon)==0) return 1;
    else return 0;
}

int isQueueFull(BarberSalon* barberSalon)
{
    if(queueSize(barberSalon)==barberSalon->qMaxSize) return 1;
    else return 0;
}

int isSleeping(BarberSalon* barberSalon)
{
    if(barberSalon->isBs==SLEEPING) return 1;
    return 0;
}

void goSleep(BarberSalon* barberSalon)
{
    barberSalon->isBs=SLEEPING;
}

void awake(BarberSalon* barberSalon)
{
    barberSalon->isBs=AWAKE;
}