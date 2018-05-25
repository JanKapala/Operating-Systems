#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "arrayDynamic.h"
#include "arrayStatic.h"

static struct option long_options[] =
{
	{"memory", 	required_argument, 	NULL, 	'm'},
	{"create", 	required_argument, 	NULL, 	'c'},
	{"search", 	required_argument, 	NULL, 	's'},
	{"remove", 	required_argument, 	NULL, 	'r'},
	{"add", 	required_argument, 	NULL, 	'a'},
	{"rad", 	required_argument, 	NULL, 	'R'}
};

int main (int argc, char **argv)
{
	//Initialization of random number generator
	time_t t;
	srand((unsigned) time(&t));

	// printf("%s", "Program has started\n");

	//Memory allocation
	int memAllocFlag = 0;
	//Dynamic allocation of flag because of 'strcpy' function requirements
	char* memAllocValue = (char*)calloc(100,sizeof(char));
	for(int i=0;i<100;i++) memAllocValue[i] = (char)0;

	//Creating
	int createFlag = 0;
	int arraySize;
	int blockSize;

	//Searching
	int searchFlag = 0;
	int searchValue;

	//Removing
	int removeFlag = 0;
	int removeNumber;

	//Adding
	int addFlag = 0;
	int addNumber;

	//Removing and Adding
	int radFlag = 0;
	int radNumber;

	//Arguments parsing
	int opt;
	while ((opt = getopt_long (argc, argv, "m:c:s:r:a:R:", long_options, NULL)) != -1)
    {
		switch (opt)
        {
		case 'm':
			memAllocFlag = 1;
			strcpy(memAllocValue, optarg);

			break;
        case 'c':
			//First argument
			createFlag = 1;
			arraySize = strtol(optarg,NULL,10);

			//Second argument
			char* secArg = argv[optind++];
			blockSize = strtol(secArg,NULL,10);

          	break;
		case 's':
			searchFlag = 1;
			searchValue = strtol(optarg,NULL,10);

			break;
		case 'r':
			removeFlag = 1;
			removeNumber = strtol(optarg,NULL,10);

			break;
		case 'a':
			addFlag = 1;
			addNumber = strtol(optarg,NULL,10);

			break;
		case 'R':
			radFlag = 1;
			radNumber = strtol(optarg,NULL,10);

			break;
		}
    }
	
	//Commands executing
	if(memAllocFlag)
	{
		if(!strcmp(memAllocValue, "static"))
		{
			// printf("%s", "Static Allocation\n");
			if(createFlag)
			{
				// printf("arraySize: %d\n", arraySize);
				// printf("blockSize: %d\n", blockSize);

				// Array creation
				ArrayStatic array;
				makeArrayStatic(&array);
				fillRandomlyArrayStaic(&array);
				// printf("%s", "Array content:\n");
				// printArrayStatic(&array);

				if(searchFlag)
				{
					// printf("searchValue: %d\n", searchValue);
					char* founded = array.array[find2Static(&array,searchValue)];
					// printf("%s", "Value in array which sum of chars is nearest to searchValue: ");
					// for(int i=0;i<BLOCK_SIZE;i++)
					// {
					// 	if(founded[i] == 0) break;
					// 	printf("%c", founded[i]);
					// }
					// printf("%c", '\n');
				}
				if(removeFlag)
				{
					// printf("Number of blocks to remove: %d\n", removeNumber);
					for(int i=0;i<removeNumber;i++) popStatic(&array);
					// printf("Array after removing: %d\n", removeNumber);
					// printArrayStatic(&array);
				}
				if(addFlag)
				{
					// printf("Number of blocks to add: %d\n", addNumber);

					for(int i=0;i<addNumber;i++)
					{
						//Making of block to add
						char block[BLOCK_SIZE];
						char randomletter;
						int capital;
						for(int i=0;i<BLOCK_SIZE;i++)
						{
							//Drawing a letter
							randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
							//Drawing capital or not
							capital = random() % 2;
							if(capital) randomletter = tolower(randomletter);
							block[i] = randomletter;
						}

						pushBackStatic(&array, block, BLOCK_SIZE-1);
					}
					// printf("Array after adding: %d\n", addNumber);
					// printArrayStatic(&array);
				}
				if(radFlag)
				{
					// printf("Number of times of adding and removing: %d\n", radNumber);
					for(int i=0;i<addNumber;i++)
					{
						//Making of block to add
						char block[BLOCK_SIZE];
						char randomletter;
						int capital;
						for(int i=0;i<BLOCK_SIZE;i++)
						{
							//Drawing a letter
							randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
							//Drawing capital or not
							capital = random() % 2;
							if(capital) randomletter = tolower(randomletter);
							block[i] = randomletter;
						}

						pushBackStatic(&array, block, BLOCK_SIZE-1);

						//Removing added block
						popStatic(&array);
					}
				}

			}
			else printf("%s", "No array creation option specified. Exiting.\n");
		}
		else
		{
			// printf("%s", "Dynamic Allocation\n");
			if(createFlag)
			{
				// printf("arraySize: %d\n", arraySize);
				// printf("blockSize: %d\n", blockSize);

				// Array creation
				ArrayDynamic* array = makeArrayDynamic(arraySize, blockSize);
				fillRandomlyArrayDynamic(array);
				// printf("%s", "Array content:\n");
				// printArrayDynamic(array);
				if(searchFlag)
				{
					// printf("searchValue: %d\n", searchValue);
					char* founded = array -> array[find2Dynamic(array,searchValue)];
					// printf("%s", "Value in array which sum of chars is nearest to searchValue: ");
					// for(int i=0;i<array->usedElements;i++)
					// {
					// 	if(founded[i] == 0) break;
					// 	printf("%c", founded[i]);
					// }
					// printf("%c", '\n');
				}
				if(removeFlag)
				{
					// printf("Number of blocks to remove: %d\n", removeNumber);
					for(int i=0;i<removeNumber;i++) popDynamic(array);
					// printf("Array after removing: %d\n", removeNumber);
					// printArrayDynamic(array);
				}
				if(addFlag)
				{
					// printf("Number of blocks to add: %d\n", addNumber);

					for(int i=0;i<addNumber;i++)
					{
						//Making of block to add
						char block[array->sizeOfBlock-1];
						char randomletter;
						int capital;
						for(int i=0;i<array->sizeOfBlock-1;i++)
						{
							//Drawing a letter
							randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
							//Drawing capital or not
							capital = random() % 2;
							if(capital) randomletter = tolower(randomletter);
							block[i] = randomletter;
						}

						pushBackDynamic(array, block, array->sizeOfBlock-1);
					}
					// printf("Array after adding: %d\n", addNumber);
					// printArrayDynamic(array);
				}
				if(radFlag)
				{
					// printf("Number of times of adding and removing: %d\n", radNumber);
					for(int i=0;i<addNumber;i++)
					{
						//Making of block to add
						char block[array->sizeOfBlock-1];
						char randomletter;
						int capital;
						for(int i=0;i<array->sizeOfBlock-1;i++)
						{
							//Drawing a letter
							randomletter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[random() % 26];
							//Drawing capital or not
							capital = random() % 2;
							if(capital) randomletter = tolower(randomletter);
							block[i] = randomletter;
						}

						pushBackDynamic(array, block, array->sizeOfBlock-1);

						//Removing added block
						popDynamic(array);
					}
				}

			}
			else printf("%s", "No array creation option specified. Exiting.\n");
		}
	}
	else printf("%s", "No memory allocation type option specified\n");
	
	return 0;
}