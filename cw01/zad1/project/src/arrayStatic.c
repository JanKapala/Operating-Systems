#include "arrayStatic.h"

//Initialize array
void makeArrayStatic(ArrayStatic* array)
{
	//Parameter checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	//Array making
	for(int i=0;i<ARRAY_SIZE;i++)
	{
		for(int j=0;j<BLOCK_SIZE;j++)
		{
			array->array[i][j]=(char)0;
		}
	}
	array -> usedElements=0;
}

//Fill whole array with specific character 
void fillArrayStatic(ArrayStatic* array, char character)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	//Filling
	for(int i=0;i<ARRAY_SIZE;i++)
	{
		for(int j=0;j<BLOCK_SIZE;j++)
		{
			array->array[i][j]=character;
		}
	}
	array -> usedElements=ARRAY_SIZE;
}

void fillRandomlyArrayStaic(ArrayStatic* array)
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
	for(int i=0;i<ARRAY_SIZE;i++)
	{
		for(int j=0;j<BLOCK_SIZE-1;j++)
		{
			//Drawing a letter
			randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
			//Drawing capital or not
			capital = random() % 2;
			if(capital) randomletter = tolower(randomletter);
			array -> array[i][j] = randomletter;
		}
		array -> array[i][BLOCK_SIZE-1] = (char)0;
	}
	array -> usedElements=ARRAY_SIZE;
}

//Print array
void printArrayStatic(ArrayStatic* array)
{
	//Parameter checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	for(int i=0;i<array->usedElements;i++)
	{
		for(int j=0;j<BLOCK_SIZE;j++)
		{
			if(array->array[i][j] == 0) break;
			printf("%c", array->array[i][j]);
		}
		printf("%c", '\n');
	}
}

//Delete all data in array, equivalent to makeStaticArray
void delArrayStatic(ArrayStatic* array)
{
	//Parameter checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	for(int i=0;i<array -> usedElements;i++)
	{
		for(int j=0;j<BLOCK_SIZE;j++)
		{
			array->array[i][j]=(char)0;
		}
	}
	array -> usedElements=0;
}

//Add new element at the back of array
void pushBackStatic(ArrayStatic* array, char* blockOfChars, int sizeOfBlockOfChars)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	if(blockOfChars == NULL)
	{
		fprintf(stderr, "Provided blockOfChars pointer is NULL pointer\n");
		return;
	}

	if(sizeOfBlockOfChars < 1 || sizeOfBlockOfChars >= BLOCK_SIZE)
	{
		fprintf(stderr, "Size of block of chars out of range");
		return;
	}

	//Check if there is memory for pushing element back
	if(array->usedElements>=ARRAY_SIZE)
	{
		fprintf(stderr, "Max size of the array has been exceeded\n");
		return;
	}

	//Inserting the element
	for(int i=0;i<BLOCK_SIZE;i++)
	{
		if(i<sizeOfBlockOfChars) array->array[array->usedElements][i]=blockOfChars[i];
		else array->array[array->usedElements][i]=(char)0;
	}
	array->usedElements++;
}

void popStatic(ArrayStatic* array)
{
	//Parameter checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return;
	}

	//Checking if there is any element to pop
	if(array->usedElements==0)
	{
		fprintf(stderr, "There is no element to pop\n");
		return;
	}

	//Poping element
	for(int i=0;i<BLOCK_SIZE;i++) array->array[array->usedElements-1][i]=(char)0;
	array->usedElements--;
}

int sumOfCharsStatic(ArrayStatic* array, int index)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return -1;
	}

	if(index < 0 || index >= array -> usedElements)
	{
		fprintf(stderr, "Index out of range");
		return -1;
	}

	//Summing
	int resultSum = 0;
	for(int i = 0; i < BLOCK_SIZE; i++)
	{
		if(array -> array[index][i]==(char)0) break;
		resultSum += (int)(array -> array[index][i]);
	}
	return resultSum;
}

int findStatic(ArrayStatic* array, int index)
{
	//Parameters checking
	if(array == NULL)
	{
		fprintf(stderr, "Provided array pointer is NULL pointer\n");
		return -1;
	}

	if(index < 0 || index >= array -> usedElements)
	{
		fprintf(stderr, "Index out of range");
		return -1;
	}

	//Finding
	int soughtSum = sumOfCharsStatic(array, index);
	int foundIndex = 0;
	int nearestSum = 0;
	int currentSum = 0;
	for(int i=0; i < array -> usedElements; i++)
	{
		if(i==index) continue;
		currentSum = sumOfCharsStatic(array, i);
		if(abs(currentSum-soughtSum) < abs(nearestSum-soughtSum))
		{
			nearestSum = currentSum;
			foundIndex = i;
		}
	}
	return foundIndex;
}

int find2Static(ArrayStatic* array, int soughtSum)
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
		currentSum = sumOfCharsStatic(array, i);
		if(abs(currentSum-soughtSum) < abs(nearestSum-soughtSum))
		{
			nearestSum = currentSum;
			foundIndex = i;
		}
	}
	return foundIndex;
}