#ifndef _RECORDSSYS_h
#define _RECORDSSYS_h

#include <stdio.h>
#include <stdlib.h>

// For system functions
#include <unistd.h>
#include <fcntl.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>

// Utilities
#include "utilities.h"

// Generate a file with an array of records - system verions
int generateSys(char* fileName, size_t recordsAmount, size_t recordSize);

// Get a record at specified index 'i' to a buffer - system version
void getRecordSys(char* buffer, size_t i, int file, size_t recordSize);

// Print array to standard output - system version
void printSys(int file, size_t recordsAmount, size_t recordSize);

// Set a record at specified index 'i' to a value specified in buffer - system version
void setRecordSys(char* buffer, size_t i, int file, size_t recordSize);

// Sort array of records in array - system version
void sortSys(int file, size_t recordsAmount, size_t recordSize);

//Copy records from one file to another using one buffer of given size only
void copySys(int fileSource, int fileDest, size_t recordsAmount, size_t recordSize);

#endif