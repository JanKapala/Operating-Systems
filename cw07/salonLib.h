#ifndef SALONLIB_H
#define SALONLIB_H

#include "common.h"

typedef struct BarberSalon
{
    int qMaxSize; // Max size of queue
    int qCurSize; // Current size of queue
    int queue[QUEUE_MAX_SIZE];
    int isBs; // is Barber Sleeping 
    long int salonID;
    long int queueID;
    int confirmation; // For driving process of invite, start haircutting, end haircutting
    int curClientID;

}BarberSalon;

BarberSalon* createBarberSalon(int maxQueueSize, char salonKey);

BarberSalon* getExistingBarberSalon(char salonKey);

void discardBarberSalon(BarberSalon* barberSalon);

void deleteBarberSalon(BarberSalon* barberSalon);

void printQueueState(BarberSalon* barberSalon);

int push(BarberSalon* barberSalon, int newElement);

int pop(BarberSalon* barberSalon);

int queueSize(BarberSalon* barberSalon);

int isQueueEmpty(BarberSalon* barberSalon);

int isQueueFull(BarberSalon* barberSalon);

int isSleeping(BarberSalon* barberSalon);

void goSleep(BarberSalon* barberSalon);

void awake(BarberSalon* barberSalon);



#endif