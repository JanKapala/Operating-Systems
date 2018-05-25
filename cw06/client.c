#include "common.h"


long int serverQueueID;
int pid;
// Buffers
msg receivedMessage;
msg sendingMessage;









// Requests
void mirrorRequest(char* stringToMirror)
{
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d:%s", pid, stringToMirror);
    sendingMessage.type = MIRROR;
    msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
}

void calcRequest(char* command)
{
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d:%s", pid, command);
    sendingMessage.type = CALC;
    msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
}

void timeRequest()
{
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d", pid);
    sendingMessage.type = TIME;
    msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
}

void endRequest()
{
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d", pid);
    sendingMessage.type = END;
    msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
    printf("Client has been stopped!\n");
}

int main(int argc, char** argv)
{
    printf("Client has been started!\n");
    // Getting path
    if(argc!=2)
    {
        printf("Invalid number of arguments\n");
        return 0;
    }
    FILE* handle = fopen(argv[1], "r");
    if(handle==NULL)
    {
        printf("Invalid file path\n");
        return 0;
    }

    // Obtaining the server queue
    long int key = getServerQueueKey();
    serverQueueID = msgget(key, 0);
    if(serverQueueID < 0)
    {
        printf("Error on server queue obtaining!\n");
        printf("ERROR: %s\n", strerror(errno));
    }

    // Making client queue
    long int clientQueueID = msgget(IPC_PRIVATE, IPC_CREAT | S_IWUSR | S_IRUSR);
    if(clientQueueID < 0)
    {
        printf("Error on client queue making!\n");
        printf("ERROR: %s\n", strerror(errno));
    }

    // Sending client queue to server
    pid = getpid();
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d:%ld", pid, clientQueueID);
    sendingMessage.type = START_CLIENT;

    int errCheck = msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
    if(errCheck < 0 )
    {
        printf("Error on client queue message sending!\n");
        printf("ERROR: %s\n", strerror(errno));
    }
    else
    {
        printf("Message with type: %ld, and content: %s has been properely sent\n", sendingMessage.type, sendingMessage.content);
    }

    // Confirmation receiving
    clrstr(receivedMessage.content);
    errCheck = msgrcv(clientQueueID, &receivedMessage, 0, pid, 0);
    if(errCheck < 0 )
    {
        printf("Error on confirmation receiving!\n");
        printf("ERROR: %s\n", strerror(errno));
    }
    else printf("Confirmation from server was obtained\n");




    
    // Messages handling

    char* lineptr = NULL;
    char* type = calloc(10,sizeof(char));
    size_t n = 0;

    while(1)
    {
        // Request sending
        clrstr(receivedMessage.content);

        lineptr = NULL;
        type = calloc(10,sizeof(char));
        n = 0;

        n = getline(&lineptr, &n, handle);
        if(n==-1)
        {
            msgctl(clientQueueID, IPC_RMID, NULL);
            printf("Client has been stopped!\n");
            return 0;
        }
    
        lineptr[n-1]='\0';
        sscanf(lineptr, "%s ", type);

        if(strcmp(type, "TIME")==0)
        {
            timeRequest();
            errCheck = msgrcv(clientQueueID, &receivedMessage, MAX_MESSAGE_LENGTH, 0, 0);
        }
        else if(strcmp(type, "CALC")==0)
        {
            calcRequest(lineptr+5);
            errCheck = msgrcv(clientQueueID, &receivedMessage, MAX_MESSAGE_LENGTH, 0, 0);
        }
        else if(strcmp(type, "MIRROR")==0)
        {
            mirrorRequest(lineptr+7);
            errCheck = msgrcv(clientQueueID, &receivedMessage, MAX_MESSAGE_LENGTH, 0, 0);
        }
        else if(strcmp(type, "END")==0 || strcmp(type, "EN")==0)
        {
            endRequest();
            free(lineptr);
            free(type);
            msgctl(clientQueueID, IPC_RMID, NULL);
            return 0;
        }
        else
        {
            printf("End of commands\n");
            free(lineptr);
            free(type);
            continue;
        }

        free(lineptr);
        free(type);

        // Request receiving
        if(errCheck < 0 )
        {
            printf("Error on response receiving!\n");
            printf("ERROR: %s\n", strerror(errno));
        }
        else
        {
            if(receivedMessage.type==MIRROR)
            {
                printf("\nMIRROR request response: ");
                printf("%s\n", receivedMessage.content);
            }
            else if(receivedMessage.type==CALC)
            {
                printf("\nCALC request response: ");
                printf("%s\n", receivedMessage.content);
            }
            else if(receivedMessage.type==TIME)
            {
                printf("\nTIME request response: ");
                printf("%s\n", receivedMessage.content);
            }
            else
            {
                printf("Type of received message is not supported\n");
            }
        }
        usleep(1000*500);
    }

    // // Response handling
    // int loadingCounter=0;
    // while(1)
    // {
    //     clrstr(receivedMessage.content);
    //     errCheck = msgrcv(clientQueueID, &receivedMessage, MAX_MESSAGE_LENGTH, 0, IPC_NOWAIT);
    //     if(errCheck < 0 )
    //     {
    //         if(errno != ENOMSG)
    //         {
    //             printf("Error on response receiving!\n");
    //             printf("ERROR: %s\n", strerror(errno));
    //         }
    //         else
    //         {
    //             // Case when no response has been receive
    //             printf("\rWaiting for a response");
    //             if(loadingCounter==0)
    //             {
    //                 printf("   ");
    //             }
    //             else
    //             {
    //                 for(int i=0;i<loadingCounter;i++) printf(".");
    //             }
    //             loadingCounter = (loadingCounter+1)%4;
    //             fflush(stdout);
    //         }
    //     }
    //     else
    //     {
    //         if(receivedMessage.type==MIRROR)
    //         {
    //             printf("\nMIRROR request response:\n");
    //             printf("%s\n", receivedMessage.content);
    //         }
    //         else if(receivedMessage.type==CALC)
    //         {
    //             printf("\nCALC request response:\n");
    //             printf("%s\n", receivedMessage.content);
    //         }
    //         else if(receivedMessage.type==TIME)
    //         {
    //             printf("\nTIME request response:\n");
    //             printf("%s\n", receivedMessage.content);
    //         }
    //         else
    //         {
    //             printf("Type of received message is not supported\n");
    //         }
    //         loadingCounter=0;
    //     }
    //     usleep(1000*500);
    // }


    // Client exiting
    clrstr(sendingMessage.content);
    snprintf(sendingMessage.content,MAX_MESSAGE_LENGTH, "%d", pid);
    sendingMessage.type = STOP_CLIENT;

    errCheck = msgsnd(serverQueueID, &sendingMessage, MAX_MESSAGE_LENGTH, IPC_NOWAIT);
    if(errCheck < 0 )
    {
        printf("Error on client stopping message sending!\n");
        printf("ERROR: %s\n", strerror(errno));
    }
    else
    {
        printf("Message with type: %ld, and content: %s has been properely sent\n", sendingMessage.type, sendingMessage.content);
    }

    msgctl(clientQueueID, IPC_RMID, NULL);
    printf("Client has been stopped!\n");
    return 0;
}