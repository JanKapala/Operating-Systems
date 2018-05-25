#ifndef _ARRAYDYNAMIC_H
#define _ARRAYDYNAMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
	int allocatedElements;
	int usedElements;
	int sizeOfBlock;
	char** array;
} ArrayDynamic;

ArrayDynamic* makeArrayDynamic(int sizeOfArray, int sizeOfBlock);

char* get(ArrayDynamic* array, int index);

void delArrayDynamic(ArrayDynamic* array);

void fillArrayDynamic(ArrayDynamic* array, char character);

void fillRandomlyArrayDynamic(ArrayDynamic* array);

void printArrayDynamic(ArrayDynamic* array);

void pushBackDynamic(ArrayDynamic* array, char* blockOfChars, int sizeOfBlockOfChars);

void popDynamic(ArrayDynamic* array);

int sumOfCharsDynamic(ArrayDynamic* array, int index);

int findDynamic(ArrayDynamic* array, int index);

int find2Dynamic(ArrayDynamic* array, int soughtSum);

#endif