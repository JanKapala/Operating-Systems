#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> // Obsluga katalogow
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <time.h>

//Little friend who can easly deals with error messages
void err_sys(const char* x) 
{ 
    perror(x); 
    exit(1); 
}

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
                voyager(subDirectory);
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
            if(buffer.st_mtime == givenTime)
            {
                printFileInfo(directory, buffer);
            }
        }
        else if(comparator == '>')
        {
            if(buffer.st_mtime > givenTime) printFileInfo(directory, buffer);
        }
    }
}

//nftw version
int printFileInfoNFTW(const char* directory, const struct stat *buffer, int fileFlags, struct FTW *pfwt)
{
    if(!S_ISREG(buffer->st_mode)) return 0;

    size_t sizeRaw = buffer->st_size;
    mode_t mode = buffer->st_mode;
    time_t atime = buffer->st_mtime;
    char* path = (char*)malloc(PATH_MAX);
    path = realpath(directory, path);
    char* size = numtostrfix(sizeRaw, 15);
    char* rights = lsperms(mode);
    char* modTime = ctime(&atime);
    int index = strlen(modTime);
    modTime[index-1]='\0';

    if(comparator == '<')
    {
        if(buffer->st_mtime < givenTime) printf("%s    %s    %s    %s\n", size, rights, modTime, path);
    }
    else if(comparator == '=')
    {
        if(buffer->st_mtime == givenTime) printf("%s    %s    %s    %s\n", size, rights, modTime, path);
    }
    else if(comparator == '>')
    {
        if(buffer->st_mtime > givenTime) printf("%s    %s    %s    %s\n", size, rights, modTime, path);
    }

    return 0;
}

time_t timeTOtime_t(int years, int months, int days, int hours, int minutes, int seconds)
{
    time_t second = (time_t)1;
    time_t minute = 60*second;
    time_t hour = 60*minute;
    time_t day = 24*hour;

    // Years counting - without ongoing
    time_t normalyear = 365*day;
    time_t leapyear = 366*day;
    time_t fourYears = 3*normalyear + leapyear;
    time_t result = (years-1)/4*fourYears;

    // Months counting - without ongoing
    int month;
    for(int i=1;i<months;i++)
    {
        if(i==1 || i==3 || i==5 || i==7 || i==8 || i==10 || i==12) month = 31*day;
        else if(i==2)
        {
            if(years%4==0) month=29*day;
            else month=28*day;
        }
        else month = 30*day;
        result += month;
    }

    // Days counting - without ongoing
    result += (days-1)*day;
    // Hours counting - without ongoing
    result += (hours-1)*hour;
    // Minutes counting - without ongoing
    result += (minutes-1)*minute;
    // Seconds counting - WITH ongoing
    result += seconds*second;

    // Z jakiegos powodu trzeba jeszcze odjac 15 dni i dodac 1 minute, wtedy dziala
    result -= 15*day;
    result += 1*minute;
    return result;
}


int main(int argc, char **argv)
{
    printf("%s", "Hello lightness my old friend\n");
    if(argc != 4) err_sys("Invalid amount of arguments");
    char* dirname = argv[1];
    if(strcmp(argv[2],"<")==0) comparator='<';
    else if(strcmp(argv[2],"=")==0) comparator='=';
    else if(strcmp(argv[2],">")==0) comparator='>';
    else err_sys("Invalid comparator");
    givenTime = (time_t)strtol(argv[3],(char **)NULL, 10);
    // printf("Data: %s, %c, %ju\n", dirname, comparator, givenTime);

    //Version
    voyager(dirname);
    // nftw(dirname, &printFileInfoNFTW, 0, FTW_PHYS);
    
}
