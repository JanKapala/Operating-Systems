#ifndef _ARRAYSTATIC_H
#define _ARRAYSTATIC_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ARRAY_SIZE 100000
#define BLOCK_SIZE 20

typedef struct
{
	char array[ARRAY_SIZE][BLOCK_SIZE];
	int usedElements;
} ArrayStatic;

void makeArrayStatic(ArrayStatic* array);

void fillArrayStatic(ArrayStatic* array, char character);

void fillRandomlyArrayStaic(ArrayStatic* array);

void printArrayStatic(ArrayStatic* array);

void delArrayStatic(ArrayStatic* array);

void pushBackStatic(ArrayStatic* array, char* blockOfChars, int sizeOfBlockOfChars);

void popStatic(ArrayStatic* array);

int sumOfCharsStatic(ArrayStatic* array, int index);

int findStatic(ArrayStatic* array, int index);

int find2Static(ArrayStatic* array, int soughtSum);

#endif