#ifndef _CONFIRMATIONS_H
#define _CONFIRMATIONS_H

#include "semLib.h"
#include "salonLib.h"

void waitForConfirmation(BarberSalon* barberSalon, long int semID);
void confirm(BarberSalon* barberSalon, long int semID);

#endif