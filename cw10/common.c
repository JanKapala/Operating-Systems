#include "common.h"

int makeMessage(char* text, int type, Message* output)
{
    if(text == NULL)
    {
        printf("makeMessage: Invalid argument, messageBuffer is NULL\n");
        return -1;
    }

    if(text == NULL)
    {
        printf("makeMessage: Invalid argument, output is NULL\n");
        return -1;
    }

    if(!(type == MT_NAME || type == MT_JOB || type == MT_COMMUNICATE || type == MT_PING || type == MT_RESULT || MT_SHUTDOWN))
    {
        printf("makeMessage: Invalid argument, type is not one of: MT_NAME, MT_JOB, MT_COMMUNICATE, MT_PING, MT_RESULT, MT_SHUTDOWN\n");
        return -1;
    }

    output->type = type;
    strcpy(output->text, text);
    return 0;
}

void printMessage(Message* message)
{
    char type[20];
    memset(type, 0, 20);
    int t = message->type;
    if(t == MT_COMMUNICATE) strcpy(type, "MT_COMMUNICATE");
    else if(t == MT_JOB) strcpy(type, "MT_JOB");
    else if(t == MT_NAME) strcpy(type, "MT_NAME");
    else if(t == MT_PING) strcpy(type, "MT_PING");
    else if(t == MT_RESULT) strcpy(type, "MT_RESULT");
    else if(t == MT_SHUTDOWN) strcpy(type, "MT_SHUTDOWN");
    else strcpy(type, "WARING: wrong type");
    
    printf("(%s) %s\n", type, message->text);
}