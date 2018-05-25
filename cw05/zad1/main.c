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
#define MAX_PROGS_IN_LINE 16

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

char** getSublines(char* string)
{
    // Result buffer initialization
    char** buffer = calloc(MAX_PROGS_IN_LINE, sizeof(char*));
    for(int i=0;i<MAX_PROGS_IN_LINE;i++)
    {
        buffer[i]=calloc(MAX_SIZE_OF_ARGUMENT*(MAX_NUMBER_OF_ARGUMENTS+1), sizeof(char));
    }

    int i=0;
    char* tok = strtok(string, "|\n");
    while(tok!=NULL)
    {
        strcpy(buffer[i], tok);
        // printf("%s\n", buffer[i]);
        tok=strtok(NULL, "|\n");
        i++;
    }
    buffer = realloc(buffer, (i+1)*sizeof(char*));
    buffer[i]=NULL;
    return buffer;
}

//
void printArrayOfStrings(char** array)
{
    int i=0;
    while(array[i]!=NULL)
    {
        // if(strcmp(array[i], "|")==0) printf("OOOoooŁŁ JeeEEAAaa...!");
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
    FILE* file = fopen(argv[1],"r");
    if(file==NULL)
    {
        printf("Can't open a file\n");
        return 0;
    }

    // Batch job interpreter
    char line[256];
    fseek(file,0,0);
    pid_t pid;
    char**  argsList;
    char* ifnext = fgets(line, 256, file); // get first line from input file
    int status;
    char** sublines;

    int fd[2];//Pipes' I/O descriptors
    pipe(fd);
    int previousOut = STDIN_FILENO;

    while(ifnext != NULL)
    {
        sublines = getSublines(line);
        int i=0;
        while(sublines[i]!=NULL) // Processing commands in pipeline
        {
            // printf("Helo\n");
            pipe(fd); // Create pipe

            pid = fork();
            if(pid==0)
            {
                // Add to pipeline
                dup2(previousOut, STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);

                printf("P");
                _exit(0);

                // Execute
                // argsList = arrayOfStrings(sublines[i]);
                // int notInPath = execvp(argsList[0], argsList);
                // if(notInPath==-1) execv(argsList[0], argsList);
                // printf("Error ocured in command: %s", sublines[i]);
                // _exit(-1);
            }
            else if(pid < 0) printf("fork() error");
            else
            {
                // previousOut = fd[0];

                waitpid(pid, &status, 0);
                if(status != 0)
                {
                    printf("Something is no yes XD\n");
                    exit(0);
                }
            }
            i++;
        }
        ifnext = fgets(line, 256, file); // Get next line from input file
    }
    dup2(STDOUT_FILENO, previousOut);







    // getSublines works
    // char* line = "prog1 arg1 ... argn1 | prog2 arg1 ... argn2 | ... | progN arg1 ... argnN";
    // char* string = strcpy(string, line); // We need dynamicaly allocated string, so it's a little trick only
    // char** words = getSublines(string);
    // printArrayOfStrings(words);








    // // Rurki works
    // printf("Hello SysOps\n");
    // // Utworzenie rurki
    // int fd[2];
    // if(pipe(fd)==0) printf("Udało się utworzyć rurkę :)\n");
    // else printf("Nie udało się utworzyć rurki :(\n");

    // // Utworzenie bufora
    // int bufferSize = 10;
    // char buffer[bufferSize];

    // // Utworzenie wiadomosci
    // char* message = "message";

    // //Log przed
    // printf("message=%s\n", message);
    // printf("buffer przed przeslaniem=%s\n", buffer);

    // //Przeslanie wiadomosci
    // write(fd[1], message, 7);
    // read(fd[0], buffer, bufferSize);

    // //Log po
    // printf("buffer po przeslaniu=%s\n", buffer);

    exit(0);
}