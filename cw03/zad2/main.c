#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>

#define MAX_NUMBER_OF_ARGUMENTS 16
#define MAX_SIZE_OF_ARGUMENT 32

//Make array of words from string
char** arrayOfStrings(char* string)
{
    // Result buffer initialization
    char** buffer = calloc(MAX_NUMBER_OF_ARGUMENTS, sizeof(char*));
    for(int i=0;i<MAX_NUMBER_OF_ARGUMENTS;i++) buffer[i]=calloc(MAX_SIZE_OF_ARGUMENT, sizeof(char));

    int i=0;
    char* tok = strtok(string, " \n");
    while(tok!=NULL)
    {
        strcpy(buffer[i], tok);
        // printf("%s\n", buffer[i]);
        tok=strtok(NULL, " \n");
        i++;
    }
    buffer = realloc(buffer, (i+1)*sizeof(char*));
    buffer[i]=NULL;
    return buffer;
}

void printArrayOfStrings(char** array)
{
    int i=0;
    while(array[i]!=NULL)
    {
        printf("%s\n", array[i]);
        i++;
    }
}

int main(int argc, char** argv)
{
    // Arguments checking
    if(argc != 2)
    {
        printf("Invalid number of arguments\n");
        exit(0);
    }

    // File openning 
    printf("%s\n", argv[1]);
    FILE* file = fopen(argv[1],"r");
    if(file==NULL)
    {
        printf("Can't open a file\n");
        return 0;
    }

    // Batch job interpreter
    char line[256];
    fseek(file,0,0);
    pid_t child_pid;
    char**  argsList;
    char* ifnext = fgets(line, 256, file);
    int status;
    while(ifnext != NULL)
    {
        child_pid = fork();
        if(child_pid==0)
        {
            argsList = arrayOfStrings(line);
            int notInPath = execvp(argsList[0], argsList);
            if(notInPath==-1) execv(argsList[0], argsList);
            printf("Error ocured in command: %s", line);
            exit(-1);
        }
        else if(child_pid < 0) printf("fork() error");
        else
        {
            waitpid(child_pid, &status, 0);
            if(status != 0) exit(0);
            ifnext = fgets(line, 256, file);
        }
    }
    exit(0);
}