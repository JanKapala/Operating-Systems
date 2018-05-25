#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h> // Obsluga katalogow
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <time.h>

time_t givenTime =100;
char comparator='>';

// Convert a mode field into "ls -l" type perms field.
static char *lsperms(int mode)
{
    static const char *rwx[] = {"---", "--x", "-w-", "-wx",
    "r--", "r-x", "rw-", "rwx"};
    static char bits[11];

    bits[0] = '-';
    strcpy(&bits[1], rwx[(mode >> 6)& 7]);
    strcpy(&bits[4], rwx[(mode >> 3)& 7]);
    strcpy(&bits[7], rwx[(mode & 7)]);
    if (mode & S_ISUID)
        bits[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        bits[6] = (mode & S_IXGRP) ? 's' : 'l';
    if (mode & S_ISVTX)
        bits[9] = (mode & S_IXOTH) ? 't' : 'T';
    bits[10] = '\0';
    return(bits);
}

// Convert size field into char* with fixed size length
char* numtostrfix(long int n, int size)
{
    char* value = (char*)malloc(size);
    sprintf(value, "%ju", n);
    int flag = 0;
    for(int i=0;i<size;i++)
    {
        if(value[i]=='\0') flag = 1;
        if(flag == 1) value[i]=' ';
    }
    value[size]='\0';
    return value;
}

//Print information about file
void printFileInfo(char* directory, struct stat buffer)
{
    size_t sizeRaw = buffer.st_size;
    mode_t mode = buffer.st_mode;
    time_t atime = buffer.st_mtime;
    char* path = (char*)malloc(PATH_MAX);
    path = realpath(directory, path);
    char* size = numtostrfix(sizeRaw, 15);
    char* rights = lsperms(mode);
    char* modTime = ctime(&atime);
    int index = strlen(modTime);
    modTime[index-1]='\0';
    printf("%s    %s    %s    %s\n", size, rights, modTime, path);
}

//Little Big friend who can (not necessary easly) deal with ex2 from lab2
void voyager(char* directory)
{
    struct stat buffer;
    lstat(directory, &buffer);

    if(S_ISDIR(buffer.st_mode))
    {
        DIR* dir;
        char* dirname = directory;
        if((dir = opendir(dirname))!=NULL)
        {
            struct dirent* dirEntry;
            while((dirEntry = readdir(dir))!=NULL)
            {
                if(strcmp(dirEntry->d_name, ".")==0) continue;
                if(strcmp(dirEntry->d_name, "..")==0) continue;

                // Making subpath
                // subDirectory = directory/subDirectoryName
                char* subDirectory = (char*)calloc(PATH_MAX, sizeof(char)); // Here a new directory is stored
                subDirectory = strcpy(subDirectory, directory);
                int subDirectoryLength = strlen(subDirectory);

                char* subDirectoryName = dirEntry->d_name; // Here a new directory's last segment is stored
                int subDirectoryNameLength = strlen(subDirectoryName);

                subDirectory[subDirectoryLength] = '/';
                for(int i=0;i<subDirectoryNameLength; i++)
                {
                    subDirectory[subDirectoryLength + 1 + i] = subDirectoryName[i];
                }

                // Recursive call
                pid_t child_pid;
                if((child_pid = fork())==0)
                {
                    voyager(subDirectory);
                    exit(0);
                }
                else waitpid(child_pid, NULL, 0);
            }
        }
        else printf("Openning failed\n");
    }
    else if(S_ISREG(buffer.st_mode))
    {
        if(comparator == '<')
        {
            if(buffer.st_mtime < givenTime) printFileInfo(directory, buffer);
        }
        else if(comparator == '=')
        {
            if(buffer.st_mtime == givenTime) printFileInfo(directory, buffer);
        }
        else if(comparator == '>')
        {
            if(buffer.st_mtime > givenTime) printFileInfo(directory, buffer);
        }
    }
}

int main(void)
{

    char* dirname = "../../..";
    voyager(dirname);

    return 0;
}