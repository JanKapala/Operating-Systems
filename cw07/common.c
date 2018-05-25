#include "common.h"

long int getKey(char projectID)
{
    // Get $HOME directory
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    //Get key
    key_t key = ftok(homedir, projectID);
    if(key==-1) printf("Error on key getting!\n");
    return key;
}

void getTimestamp(char* result)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    sprintf(result, "timestamp: %ld.%ld", tp.tv_sec, tp.tv_nsec);
}