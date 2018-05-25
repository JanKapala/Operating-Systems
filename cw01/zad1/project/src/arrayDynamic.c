#include "arrayDynamic.h"

//Allocate memory for array of specified number of blocks (sizeOfArray)
//and size of blocks (sizeOfBlock)
ArrayDynamic* makeArrayDynamic(int sizeOfArray, int sizeOfBlock)
{
	char** array = calloc(sizeOfArray, sizeof(char*));
	if(array == NULL)
	{
		fprintf(stderr, "%s", "Calloc memory allocation error. Exiting.");
		return NULL;
	}
	for(int i=0;i<sizeOfArray;i++)
	{
		array[i] = (char*)calloc(sizeOfBlock,sizeof(char));
		if(array[i] == NULL)
		{
			fprintf(stderr, "%s", "Calloc memory allocation error. Exiting.");
			return NULL;
		}
	}
	ArrayDynamic* result = (ArrayDynamic*)calloc(1, sizeof(ArrayDynamic));
	if(result == NULL)
	{
		fprintf(stderr, "%s", "Calloc memory allocation error. Exiting.");
		return NULL;
	}
	result -> allocatedElements = sizeOfArray;
	result -> usedElements = 0;
	result -> sizeOfBlock = sizeOfBlock;
	result -> array = array;
	return result;
}

//Get pointer to the first char of block which index have you provided
char* get(ArrayDynamic* array, int index)
{
	if(index < 0 || index >= array -> usedElements)
	{
		fprintf(stderr, "Provided index is out of range. Max index is: %d. NULL returned. \n", array -> usedElements-1);
		return NULL;
	}
	return array -> array[index];
}

void delArrayDynamic(ArrayDynamic* array)
{
	for(int i=0; i < array -> allocatedElements;i++) free(array -> array[i]);
	free(array -> array);
	free(array);
	array = NULL;
}

//Fill whole allocated array of chars with specified character
void fillArrayDynamic(ArrayDynamic* array, char character)
{
	if(array == NULL)
	array->usedElements=array->allocatedElements;
	for(int i=0;i<array->usedElements;i++)
	{
		for(int j=0;j<array->sizeOfBlock;j++) array->array[i][j] = character;
	}
}

void fillRandomlyArrayDynamic(ArrayDynamic* array)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	//Filling
	char randomletter;
	int capital;
	for(int i=0;i < array -> allocatedElements;i++)
	{
		for(int j=0;j<array->sizeOfBlock-1;j++)
		{
			//Drawing a letter
			randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
			//Drawing capital or not
			capital = random() % 2;
			if(capital) randomletter = tolower(randomletter);
			array -> array[i][j] = randomletter;
		}
		array -> array[i][array->sizeOfBlock-1] = (char)0;
	}
	array -> usedElements= array -> allocatedElements;
}


//Print array of chars without using "%s"
void printArrayDynamic(ArrayDynamic* array)
{
	for(int i=0;i<array->usedElements;i++)
	{
		for(int j=0;j<array->sizeOfBlock;j++)
		{
			if(array->array[i][j]==(char)0) break;
			printf("%c",array->array[i][j]);
		}
		printf("%c",'\n');
	}
}

//Push new element after the last existing element
void pushBackDynamic(ArrayDynamic* array, char* blockOfChars, int sizeOfBlockOfChars)
{
	//Check for validity of inserted block of chars
	if(sizeOfBlockOfChars < 1)
	{
		char errorText[256] = "Provided sizeOfBlockOfChars is smaller than 1. Operation rejected\n";
		fprintf(stderr, "%s", errorText);
		return;
	}

	if(sizeOfBlockOfChars > array -> sizeOfBlock)
	{
		char errorText[256] = "You tried to insert block of chars which size is %d, but max valid size of block is %d\n";
		fprintf(stderr, errorText, sizeOfBlockOfChars,array->sizeOfBlock);
		return;
	}

	//Allocate additional memory space if required
	if(array -> usedElements == array -> allocatedElements)
	{
		array -> allocatedElements *= 2;
		array -> array = realloc(array -> array, (array -> allocatedElements) * sizeof(char*));
		if(array -> array == NULL)
		{
			fprintf(stderr, "%s", "Realloc memory allocation error. Exiting.");
			return;
		}
		for(int i = array -> usedElements; i < array -> allocatedElements; i++)
		{
			array->array[i]=(char*)calloc(array->sizeOfBlock, sizeof(char));
			if(array->array[i] == NULL)
			{
				fprintf(stderr, "%s", "Realloc memory allocation error. Exiting.");
				return;
			}
		}
	}

	//Push at the end of used part of the array
	for(int i=0;i<sizeOfBlockOfChars;i++)
	{
		array->array[array -> usedElements][i] = blockOfChars[i];
	}
	array -> usedElements++;
}

//Delete the last existing element
void popDynamic(ArrayDynamic* array)
{
	//Check if empty
	if(array -> usedElements < 1) return;
	//If used size decreased significantly then it should be reallocated
	if(array -> usedElements-1 <= (array -> allocatedElements)/4.0)
	{
		//Deallocation
		for(int i = (array -> allocatedElements)/2; i < array -> allocatedElements; i++)
		{
			free(array->array[i]);
		}
		array -> array = realloc(array -> array, (array -> allocatedElements)/2 * sizeof(char*));
		array -> allocatedElements = (array -> allocatedElements)/2;
	}

	//Destroying content of last block of chars
	free(array -> array[array -> usedElements-1]);
	array -> array[array -> usedElements-1] = (char*)calloc(array -> sizeOfBlock, sizeof(char));

	//Update atribute
	array -> usedElements--;
}

//Get the sum of chars(as numbers) from the element pointed by provided index in provided array
int sumOfCharsDynamic(ArrayDynamic* array, int index)
{
	//Cheking for validity of index
	if(index < 0 || index >= array -> usedElements)
	{
		char errorText[256] = "Provided index is out of range. Operation rejected\n";
		fprintf(stderr, "%s", errorText);
		return -1;
	}

	int resultSum = 0;
	for(int i = 0; i < array -> sizeOfBlock; i++) resultSum += (int)(array -> array[index][i]);
	return resultSum;
}

int findDynamic(ArrayDynamic* array, int index)
{
	int soughtSum = sumOfCharsDynamic(array, index);
	int foundIndex = 0;
	int nearestSum = 0;
	int currentSum = 0;
	for(int i=0; i < array -> usedElements; i++)
	{
		if(i==index) continue;
		currentSum = sumOfCharsDynamic(array, i);
		if(abs(currentSum-soughtSum) < abs(nearestSum-soughtSum))
		{
			nearestSum = currentSum;
			foundIndex = i;
		}
	}
	return foundIndex;
}

int find2Dynamic(ArrayDynamic* array, int soughtSum)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return -1;
	}

	//Finding
	int foundIndex = 0;
	int nearestSum = 0;
	int currentSum = 0;
	for(int i=0; i < array -> usedElements; i++)
	{
		currentSum = sumOfCharsDynamic(array, i);
		if(abs(currentSum-soughtSum) < abs(nearestSum-soughtSum))
		{
			nearestSum = currentSum;
			foundIndex = i;
		}
	}
	return foundIndex;
}