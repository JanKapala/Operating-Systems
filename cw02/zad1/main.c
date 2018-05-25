#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// For system functions
#include <unistd.h>
#include <fcntl.h>

// Records libs (system and library variants)
#include "recordsSys.h"
#include "recordsLib.h"

#include <getopt.h>
#include <string.h>
#define SYS 0
#define LIB 1

static struct option long_options[] =
{
	{"variant",     required_argument, 	NULL, 	'v'},
    {"generate",    required_argument, 	NULL, 	'g'},
    {"dimensions",  required_argument, 	NULL, 	'd'},
	{"sort", 	    required_argument, 	NULL, 	's'},
	{"copy", 	    required_argument, 	NULL, 	'c'}
};

int main(int argc, char **argv)
{
    // Random engine initialization
    srand(time(NULL));

    // // Program parameters
    // char fileName[] = "theFile.txt";
    // size_t recordsAmount = 100;
    // size_t recordSize = 8192;// 4, 512, 4096, 8192 

    // //SYS
    // //GEN
    // int file = generateSys(fileName, recordsAmount, recordSize);

    // //SORT
    // // sortSys(file, recordsAmount, recordSize);

    //COPY
    // int file2;
    // if((file2 = open("theFile2.txt",  O_RDWR | O_TRUNC | O_CREAT, 0666)) < 0)
    // {
    //     printf("%s", "File opening error.");
    // }
    // copySys(file, file2, recordsAmount, recordSize);

    // LIB
    // FILE* file = generateLib(fileName, recordsAmount, recordSize);
    // printLib(file, recordsAmount, recordSize);
    // sortLib(file, recordsAmount, recordSize);
    // printLib(file, recordsAmount, recordSize);

    // FILE* file2;
    // if((file2 = fopen("theFile2.txt", "w+b")) == NULL)
    // {
    //     printf("%s", "File opening error.");
    // }
    // copyLib(file, file2, recordsAmount, recordSize);


    //Variant
    int variantFlag = SYS;

	//Generate
	int generateFlag = 0;
	char* generateFilePath = (char*)calloc(100,sizeof(char));
	for(int i=0;i<100;i++) generateFilePath[i] = (char)0;

	//Dimensions
    size_t recordsAmount = 10;
	size_t recordSize  = 2;

	//Sort
	int sortFlag = 0;
	char* sortFilePath = (char*)calloc(100,sizeof(char));
	for(int i=0;i<100;i++) sortFilePath[i] = (char)0;

	//Copy
	int copyFlag = 0;
	char* copyFilePathSource = (char*)calloc(100,sizeof(char));
	for(int i=0;i<100;i++) copyFilePathSource[i] = (char)0;
    char* copyFilePathDest = (char*)calloc(100,sizeof(char));
	for(int i=0;i<100;i++) copyFilePathDest[i] = (char)0;

    //Arguments parsing
	int opt;
	while ((opt = getopt_long (argc, argv, "v:g:d:s:c:", long_options, NULL)) != -1)
    {
		switch (opt)
        {
		case 'v':
            if(strcmp(optarg,"sys") == 0)
            {
                variantFlag = SYS;
                printf("%s", "Variant = SYS\n");
            }
            else if(strcmp(optarg,"lib") == 0)
            {
                variantFlag = LIB;
                printf("%s", "Variant = LIB\n");
            }
            else
            {
                printf("%s", "Invalid variant. Exiting\n");
                exit(EXIT_FAILURE);
            }

			break;
        case 'g':
            generateFlag = 1;
			strcpy(generateFilePath, optarg);
            printf("Path to make the file: %s\n", generateFilePath);

			break;
        case 'd':
			//First argument
			recordsAmount = strtol(optarg,NULL,10);

			//Second argument
			char* secArg = argv[optind++];
			recordSize = strtol(secArg,NULL,10);

            printf("RecordsAmount: %ld\n", recordsAmount);
            printf("RecordSize: %ld\n", recordSize);
          	break;
		
		case 's':
			sortFlag = 1;
			strcpy(sortFilePath, optarg);
            printf("Path to file to sort: %s\n", sortFilePath);

			break;
		case 'c':
			copyFlag = 1;
            //First argument
			strcpy(copyFilePathSource, optarg);
            printf("Path to source file: %s\n", copyFilePathSource);

            //Second argument
            char* nextArg = argv[optind++];
            strcpy(copyFilePathDest, nextArg);
            printf("Path to destination file: %s\n", copyFilePathDest);


			break;
        }
    }

    //Commands executing
    //Generating
    if(generateFlag)
    {
        int file = generateSys(generateFilePath, recordsAmount, recordSize);
        close(file);
    }

    //Sorting
    if(sortFlag)
    {
        if(variantFlag == SYS)
        {
            // File opening
            int file;
            if((file = open(sortFilePath, O_RDWR | O_TRUNC | O_CREAT, 0666)) < 0)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }
            sortSys(file, recordsAmount, recordSize);
        }
        else if(variantFlag == LIB)
        {
            // File opening
            FILE* file;
            if((file = fopen(sortFilePath,"w+t")) == NULL)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }
            printf("filepath %s\n", sortFilePath);
            printf("recordsAmount %ld\n", recordsAmount);
            printf("recordSize %ld\n", recordSize);
            sortLib(file, recordsAmount, recordSize);
        }
        else
        {
            printf("%s", "Invalid variant. Exiting\n");
            exit(EXIT_FAILURE);
        }
    }

    //Copying
    if(copyFlag)
    {
        if(variantFlag == SYS)
        {
            // Files opening
            int fileSource;
            if((fileSource = open(copyFilePathSource, O_RDWR | O_TRUNC | O_CREAT, 0666)) < 0)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }

            int fileDest;
            if((fileDest = open(copyFilePathDest, O_RDWR | O_TRUNC | O_CREAT, 0666)) < 0)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }

            //Copying execution
            copySys(fileSource, fileDest, recordsAmount, recordSize);
        }
        else if(variantFlag == LIB)
        {
            // Files opening
            FILE* fileSource;
            if((fileSource = fopen(copyFilePathSource,"w+t")) == NULL)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }

            FILE* fileDest;
            if((fileDest = fopen(copyFilePathDest,"w+t")) == NULL)
            {
                printf("%s", "File opening error.");
                exit(EXIT_FAILURE);
            }

            //Copying execution
            copyLib(fileSource, fileDest, recordsAmount, recordSize);
        }
        else
        {
            printf("%s", "Invalid variant. Exiting\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}