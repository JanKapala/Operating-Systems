#ifndef _RECORDSLIB_H
#define _RECORDSLIB_H

#include <stdio.h>
#include <stdlib.h>

// For system functions
#include <unistd.h>
#include <fcntl.h>

// Utilities
#include "utilities.h"


// Generate a file with an array of records - library verions
FILE* generateLib(char* fileName, size_t recordsAmount, size_t recordSize);

// Get a record at specified index 'i' to a buffer - library version
void getRecordLib(char* buffer, size_t i, FILE* file, size_t recordSize);

// Print array to standard output - library version
void printLib(FILE* file, size_t recordsAmount, size_t recordSize);

// Set a record at specified index 'i' to a value specified in buffer - library version
void setRecordLib(char* buffer, size_t i, FILE* file, size_t recordSize);

// Sort array of records in array - library version
void sortLib(FILE* file, size_t recordsAmount, size_t recordSize);

//Copy records from one file to another using one buffer of given size only
void copyLib(FILE* fileSource, FILE* fileDest, size_t recordsAmount, size_t recordSize);

#endif