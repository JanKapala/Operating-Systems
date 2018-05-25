#include "common.h"

long int getServerQueueKey()
{
    // Get $HOME directory
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    //Get key
    key_t key = ftok(homedir, PROJECT_ID);

    return key;
}

void clrstr(char* stringToClear)
{
    for(int i=0;i<MAX_MESSAGE_LENGTH;i++) stringToClear[i]='\0';
}