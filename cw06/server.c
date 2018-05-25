
#include "common.h"

long int serverQueueID;

long int clients[MAX_CLIENTS_AMOUNT][2];
int clientCounter = 0;

void clientsArrayInitialize()
{
    for(int i=0;i<MAX_CLIENTS_AMOUNT;i++)
    {
        clients[i][0]=-1;
        clients[i][1]=-1;
    }
}

void addClient(int clientPID, int clientQueueID)
{
    if(clientCounter==MAX_CLIENTS_AMOUNT)
    {
        printf("Error, max suported amount of clients is %d, no client has been added\n", MAX_CLIENTS_AMOUNT);
        return;
    }
    // Check if queue for this clientPID already is opened
    for(int i=0; i<MAX_CLIENTS_AMOUNT;i++)
    {
        if(clients[i][0]==clientPID)
        {
            printf("Error, client: %d already has opened queue on server", clientPID);
            return;
        }
    }

    // If not then open it
    for(int i=0; i<MAX_CLIENTS_AMOUNT;i++)
    {
        if(clients[i][0]==-1)
        {
            clients[i][0]=clientPID;
            clients[i][1]=clientQueueID;
            msgget(clientQueueID, 0);
            clientCounter++;
            return;
        }
    }
}

void printClients()
{
    printf("LIST OF CLIENTS:\n\n");
    if(clientCounter==0)
    {
        printf("No clients:\n\n");
    }
    for(int i=0;i<MAX_CLIENTS_AMOUNT;i++)
    {
        if(clients[i][0]!=-1)
        {
            printf("Client PID: %ld\n", clients[i][0]);
            printf("Queue key: %ld\n\n", clients[i][1]);
        }
    }
    printf("\n\n");
}

void deleteClient(int clientPID)
{
    for(int i=0; i<MAX_CLIENTS_AMOUNT;i++)
    {
        if(clients[i][0]==clientPID)
        {
            clients[i][0]=-1;
            clients[i][1]=-1;
            return;
        }
    }
}

long int findClientQueueID(int clientPID)
{
    for(int i=0; i<MAX_CLIENTS_AMOUNT;i++)
    {
        if(clients[i][0]==clientPID) return clients[i][1];
    }
    printf("Error: no client with given clientPID\n");
    return -1;
}

int mirror(char* inputString, char* outputString)
{
    int size=0;
    while(inputString[size]!='\0') size++;
    char tempString[size+1];
    for(int i=0;i<size;i++) tempString[size-i-1]=inputString[i];
    tempString[size]='\0';
    strcpy(outputString, tempString);
    return size;
}

int calc(char* command, int arg1, int arg2, char* outputString)
{
    int result;
    if(strcmp(command, "ADD")==0) result = arg1+arg2;
    else if(strcmp(command, "MUL")==0) result = arg1*arg2;
    else if(strcmp(command, "SUB")==0) result = arg1-arg2;
    else if(strcmp(command, "DIV")==0) result = arg1/arg2;
    else
    {
        printf("Error: command provided to calc is invalid\n");
        return -1;
    }
    sprintf(outputString, "%d", result);
    return strlen(outputString);
}

void stopHandler(int signum)
{
    
    msgctl(serverQueueID, IPC_RMID, NULL);
    printf("\nServer has been stopped!\n");
    exit(0);
}

int main(void)
{
    // ----------------SERVER STARTING----------------
    // Interruption handlers
    signal(SIGINT, stopHandler);
    signal(SIGTSTP, stopHandler);
    // Server queue making
    long int key= getServerQueueKey();
    serverQueueID = msgget(key, IPC_CREAT | S_IWUSR | S_IRUSR);//without IPC_EXCL because we want to overwrite existing server queue
    if(serverQueueID < 0)
    {
        printf("Error on server queue making!\n");
        printf("ERROR: %s\n", strerror(errno));
    }
    printf("Server has been started!\n");
    
    clientsArrayInitialize();


    // ----------------MESSAGES HANDLING----------------
    msg receivedMessage;
    msg sendingMessage;

    int errCheck;
    int loadingCounter=0;


    int ifEnd=0;
    // Communication loop
    while(1)
    {
        errCheck = msgrcv(serverQueueID, &receivedMessage, MAX_MESSAGE_LENGTH, 0, IPC_NOWAIT);
        if(errCheck < 0 )
        {
            if(errno != ENOMSG)
            {
                printf("Error on message receiving!\n");
                printf("ERROR: %s\n", strerror(errno));
            }
            else
            {
                // Case when no message has been receive

                printf("\rWaiting for a message");
                if(loadingCounter==0)
                {
                    printf("   ");
                }
                else
                {
                    for(int i=0;i<loadingCounter;i++) printf(".");
                }
                loadingCounter = (loadingCounter+1)%4;
                fflush(stdout);
            }
        }
        else
        {
            // Message handling
            // printf("\nReceived message of type: %ld and content: %s\n", receivedMessage.type, receivedMessage.content);
            if(receivedMessage.type==START_CLIENT)
            {
                int clientPID;
                long int clientQueueID;
                sscanf(receivedMessage.content, "%d:%ld", &clientPID, &clientQueueID);

                addClient(clientPID, clientQueueID);
                printf("New client (PID: %d, queue key: %ld) has been added\n", clientPID, clientQueueID);

                //printClients();

                //Confirmation message
                clrstr(sendingMessage.content);
                sendingMessage.type = clientPID;
                msgsnd(clientQueueID, &sendingMessage, 0, 0);
            }
            else if(receivedMessage.type==STOP_CLIENT)
            {
                int clientPID;
                sscanf(receivedMessage.content, "%d", &clientPID);
                deleteClient(clientPID);
                printf("Client (PID: %d) has been deleted\n", clientPID);
            }
            else if(receivedMessage.type==MIRROR)
            {
                int clientPID;
                char messageToMirror[MAX_MESSAGE_LENGTH];
                sscanf(receivedMessage.content,"%d:%s", &clientPID, messageToMirror);
                printf("Request MIRROR was received. Client PID: %d, message to mirror: %s\n", clientPID, messageToMirror);

                long int clientQueueID = findClientQueueID(clientPID);

                clrstr(sendingMessage.content);
                int msgsz = mirror(messageToMirror, sendingMessage.content);
                sendingMessage.type = MIRROR;

                msgsnd(clientQueueID, &sendingMessage, msgsz, IPC_NOWAIT);
            }
            else if(receivedMessage.type==CALC)
            {
                int clientPID;
                char command[3];
                int arg1;
                int arg2;
                sscanf(receivedMessage.content,"%d:%s %d %d", &clientPID, command, &arg1, &arg2);
                printf("Request CALC was received. Client PID: %d, message to calc: %s %d %d\n", clientPID, command, arg1, arg2);

                long int clientQueueID = findClientQueueID(clientPID);

                clrstr(sendingMessage.content);
                int msgsz = calc(command, arg1, arg2, sendingMessage.content);
                printf("Calc result: %s", sendingMessage.content);
                sendingMessage.type = CALC;

                msgsnd(clientQueueID, &sendingMessage, msgsz, IPC_NOWAIT);
            }
            else if(receivedMessage.type==TIME)
            {
                int clientPID;
                sscanf(receivedMessage.content,"%d", &clientPID);
                long int clientQueueID = findClientQueueID(clientPID);

                printf("Request TIME was received. Client PID: %d\n", clientPID);

                time_t rawtime;
                struct tm * timeinfo;
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
                clrstr(sendingMessage.content);
                snprintf(sendingMessage.content, MAX_MESSAGE_LENGTH, "Current server time and date: %s", asctime(timeinfo));
                sendingMessage.type = TIME;

                msgsnd(clientQueueID, &sendingMessage, strlen(sendingMessage.content), IPC_NOWAIT);

            }
            else if(receivedMessage.type==END)
            {
                ifEnd=1;
                msgctl(serverQueueID, IPC_RMID, NULL);
                printf("\nServer has been stopped!\n");
                exit(0);
            }
            else
            {
                printf("Type of received message is not supported\n");
            }
            loadingCounter=0;
        }
        usleep(1000*500);
    }

    // ----------------SERVER STOPPING----------------
    msgctl(serverQueueID, IPC_RMID, NULL);
    printf("Server has been stopped!\n");
    exit(0);
}

                // // check if end request has been received
                // if(ifEnd==1)
                // {
                //     msgctl(serverQueueID, IPC_RMID, NULL);
                //     printf("Server has been stopped!\n");
                //     exit(0);
                // }
                // else // if not then wait for next message
                // {