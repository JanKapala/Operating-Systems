#include <time.h>
#include <sys/epoll.h>
#include <pthread.h>

#include "common.h"

int lserverSocketFD;
int nserverSocketFD;
int port;
pthread_t listenning_thread;
pthread_t director_thread;
pthread_t ping_thread;
long long calculationNumber = 0;
char unix_socket_file_path[91]; // better shorter than 92 (max size on some unix systems)
int monitor;

// CLIENTS
typedef struct Client
{
    int sockfd;
    char name[CLIENT_NAME_MAX_LEN];
} Client;

Client* clients[MAX_CLIENTS_NUMBER];
int currentClientsNumber;
int nextClientCounter;

void initClients()
{
    for(int i=0; i< MAX_CLIENTS_NUMBER; i++) clients[i] = NULL;
    currentClientsNumber = 0;
    nextClientCounter = 0;
}

int findClient(char* name)
{
    for(int i=0; i<MAX_CLIENTS_NUMBER; i++)
    {
        if(clients[i]==NULL) continue;
        if(strcmp(clients[i]->name, name) == 0) return i;
    }
    return -1;
}

int findClientBySockFD(int sockfd)
{
    for(int i=0; i<MAX_CLIENTS_NUMBER; i++)
    {
        if(clients[i]==NULL) continue;
        if(clients[i]->sockfd == sockfd) return i;
    }
    return -1;
}

void printClients()
{
    for(int i=0; i< MAX_CLIENTS_NUMBER; i++)
    {
        if(clients[i]==NULL) continue;
        printf("Client name: %s, sockfd: %d\n", clients[i]->name, clients[i]->sockfd);
    }
}

void registerClient(char* name, int clientSocketFD)
{
    if(currentClientsNumber == MAX_CLIENTS_NUMBER) { printf("There is no place for new client\n"); return;}

    int newClientIndex = findClient(name);
    if(newClientIndex != -1) // Client already registrated
    {
        printf("Registration attempt. Client with name '%s' is already registered. Canceling\n", name);

        // Response sending
        Message buffer;
        makeMessage("Client with such name is already registered. Canceling", MT_SHUTDOWN, &buffer);
        int n = write(clientSocketFD, &buffer, sizeof(buffer));
        if(n < 0) { perror("ERROR writing to socket"); exit(1); }

        // Disconnecting
        int resp = shutdown(clientSocketFD, SHUT_WR);
        if(resp < 0) { perror("ERROR on shutdown"); exit(0); }
    }
    else // New client registrating
    {
        // Empty place finding
        newClientIndex = 0;
        while(clients[newClientIndex] != NULL) newClientIndex++;

        // Client structure making
        clients[newClientIndex] = calloc(1, sizeof(Client));
        strcpy(clients[newClientIndex]->name, name);
        clients[newClientIndex]->sockfd = clientSocketFD;
        currentClientsNumber++;

        // Epoll registrating
        struct epoll_event epollEvent;
        epollEvent.events = EPOLLIN;
        epollEvent.data.fd = clientSocketFD;
        int resp = epoll_ctl(monitor, EPOLL_CTL_ADD, clientSocketFD, &epollEvent);
        if(resp != 0) { perror("ERROR on epoll_ctl"); exit(0);}

        // Response sending
        Message buffer;
        makeMessage("Client registrated successfully", MT_COMMUNICATE, &buffer);
        int n = write(clientSocketFD, &buffer, sizeof(buffer));
        if(n < 0) { perror("ERROR writing to socket"); exit(1); }

        printf("Client '%s' registrated successfully\n", name);
    }
}

int getNextClient() // Function ONLY for choosing client for job (it increments global counter)
{
    int startIndex = nextClientCounter;
    while(clients[nextClientCounter] == NULL)
    {
        nextClientCounter = (nextClientCounter+1)%MAX_CLIENTS_NUMBER;
        if(startIndex == nextClientCounter) return -1; // Means that ther is no next client
    }
    int temp = nextClientCounter;
    nextClientCounter = (nextClientCounter+1)%MAX_CLIENTS_NUMBER;
    return temp;
}

void deleteClient(char* name)
{
    int clientIndex = findClient(name);
    if(clientIndex != -1)
    {
        close(clients[clientIndex]->sockfd);
        epoll_ctl(monitor, EPOLL_CTL_DEL, clients[clientIndex]->sockfd, NULL);
        free(clients[clientIndex]);
        clients[clientIndex] = NULL;
        currentClientsNumber--;
    }
    else
    {
        printf("ERROR on client deletion: no such client\n");
        exit(0);
    }
}

void deleteClientBySockFD(int sockfd)
{
    int clientIndex = findClientBySockFD(sockfd);
    if(clientIndex != -1)
    {
        shutdown(clients[clientIndex]->sockfd, SHUT_RD);
        epoll_ctl(monitor, EPOLL_CTL_DEL, clients[clientIndex]->sockfd, NULL);
        free(clients[clientIndex]);
        clients[clientIndex] = NULL;
        currentClientsNumber--;
    }
    else
    {
        printf("ERROR on client deletion: no such client\n");
        exit(0);
    }
}

// SIGNAL HANDLING
void sigintHandler(int signum)
{
    printf("\nSIGINT received\n" );
    // send shutdown message
    Message buffer;
    makeMessage("shutdown", MT_SHUTDOWN, &buffer);
    for(int i=0;i<MAX_CLIENTS_NUMBER;i++)
    {
        if(clients[i]==NULL) continue;
        int n = write(clients[i]->sockfd, &buffer, sizeof(buffer));
        if(n < 0) { perror("ERROR writing to socket"); exit(0); }
        shutdown(clients[i]->sockfd, SHUT_WR);
        deleteClientBySockFD(clients[i]->sockfd);
    }
    exit(0);
}

// NETWORK HANDLING

void localSocketInit()
{
    // Socket creation
    lserverSocketFD = socket(AF_UNIX, SOCK_STREAM, 0);
    if(lserverSocketFD < 0) { perror("ERROR opening socket (LOCAL)"); exit(1); }

    // Server address initialization
    struct sockaddr_un lserverAddress;
    memset(&lserverAddress, 0, sizeof(lserverAddress));
    lserverAddress.sun_family = AF_UNIX;
    unlink(unix_socket_file_path);
    strcpy(lserverAddress.sun_path, unix_socket_file_path);
    
    // Server address binding
    int lbindResponse = bind(lserverSocketFD, (struct sockaddr *)&lserverAddress, sizeof(lserverAddress));
    if(lbindResponse < 0) { perror("ERROR on binding (LOCAL)"); exit(1); }
    
    // Start listenning
    int llistenResponse = listen(lserverSocketFD, MAX_CLIENTS_NUMBER);
    if(llistenResponse < 0) { perror("ERROR on listenning (LOCAL)"); exit(0); }

    // Adding to the epoll
    struct epoll_event lepollEvent;
    lepollEvent.events = EPOLLIN;
    lepollEvent.data.fd = lserverSocketFD;
    epoll_ctl(monitor, EPOLL_CTL_ADD, lserverSocketFD, &lepollEvent);
}

void netSocketInit()
{
    // Socket creation
    nserverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(nserverSocketFD < 0) { perror("ERROR opening socket (NET)"); exit(1); }
    
    // Server address initialization
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Server address binding
    int bindResponse = bind(nserverSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if(bindResponse < 0) { perror("ERROR on binding (NET)"); exit(1); }
    
    // Start listenning
    int listenResponse = listen(nserverSocketFD, MAX_CLIENTS_NUMBER);
    if(listenResponse < 0) { perror("ERROR on listenning (NET)"); exit(0); }

    // Adding to the epoll
    struct epoll_event epollEvent;
    epollEvent.events = EPOLLIN;
    epollEvent.data.fd = nserverSocketFD;
    epoll_ctl(monitor, EPOLL_CTL_ADD, nserverSocketFD, &epollEvent);
}

void* listenning(void* _arg)
{
    struct epoll_event currentEvent;
    int clientSocketFD;
    int serverSocketFD;
    while(1)
    {
        epoll_wait(monitor, &currentEvent, 1, -1);
        if(currentEvent.data.fd == lserverSocketFD || currentEvent.data.fd == nserverSocketFD)
        {
            // Connection accepting
            serverSocketFD = currentEvent.data.fd;
            clientSocketFD = accept(serverSocketFD, NULL, NULL);
            if(clientSocketFD < 0) { perror("ERROR on accept"); exit(1); }

            // Name message receiving
            Message buffer;
            memset(&buffer, 0, sizeof(Message));
            int n = read(clientSocketFD, &buffer, sizeof(buffer));
            if(n < 0) { perror("ERROR reading from socket"); exit(1); }

            // Message handling
            if(buffer.type==MT_NAME) registerClient(buffer.text, clientSocketFD);
            else
            {
                printf("Received invalid first message, its type should be 'MT_NAME\n");
                printMessage(&buffer);
            }
        }
        else // clients' sockets handling
        {
            clientSocketFD = currentEvent.data.fd;

            // Message receiving
            Message buffer;
            memset(&buffer, 0, sizeof(buffer));
            int n = read(clientSocketFD, &buffer, sizeof(buffer));
            if(n < 0) { perror("ERROR reading from socket"); exit(1); }

            // Message handling
            if(buffer.type == MT_SHUTDOWN)
            {
                int index;
                for(index=0; index < MAX_CLIENTS_NUMBER; index++)
                {
                    if(clients[index]==NULL) continue;
                    if(clients[index]->sockfd==clientSocketFD) break;
                }
                printf("Client '%s' has been deregistrated\n", clients[index]->name);
                deleteClientBySockFD(clientSocketFD);
            }
            else if(buffer.type == MT_RESULT) printf("%s\n", buffer.text);
            else if(buffer.type == MT_PING) continue;
            else printMessage(&buffer);
        }
    }
}

void* director(void* _arg)
{
    while(1)
    {
        // Getting input data from user
        printf("%lld: Type calculation:\n", calculationNumber);
        char temp[MESSAGE_TEXTBUFFER_SIZE];
        char textbuffer[MESSAGE_TEXTBUFFER_SIZE];
        fgets(temp, MESSAGE_TEXTBUFFER_SIZE, stdin);
        sprintf(textbuffer, "%lld: %s", calculationNumber, temp);

        // Job-message making
        Message message;
        makeMessage(textbuffer, MT_JOB, &message);

        // Client choosing
        int clientIndex = getNextClient();
        if(clientIndex == -1)
        {
            printf("No clients, withdrawning\n");
            continue;
        }
        calculationNumber = (calculationNumber+1)%__LONG_LONG_MAX__;

        // Job-message sending
        int n = write(clients[clientIndex]->sockfd, &message, sizeof(message));
        if(n < 0) { perror("ERROR writing to socket"); exit(1); }
    }
}

void* ping()
{
    Message pingMessage, response;
    makeMessage("PING", MT_PING, &pingMessage);
    while(1)
    {
        sleep(1);
        for(int i=0; i<MAX_CLIENTS_NUMBER;i++)
        {
            if(clients[i] != NULL)
            {
                // Ping sending
                int n = write(clients[i]->sockfd, &pingMessage, sizeof(pingMessage));
                if(n < 0) { perror("ERROR writing to socket"); exit(1); }

                memset(&response, 0, sizeof(response));
                n = recv(clients[i]->sockfd, &response, sizeof(response), MSG_DONTWAIT);
                if(n < 0) { perror("ERROR reading from socket"); exit(1); }

                if(response.type=MT_PING)
                {
                    // TODO
                }
                else
                {
                    // TODO
                }
            } 
        }
    }
}

int main( int argc, char** argv )
{
    // Arguments parsing
    if(argc != 3) {printf("Invalid number of arguments\n"); exit(1);}
    port = strtol(argv[1], NULL, 10);
    if(port < 1024 || port > 65535)
    {
        printf("Invalid port number, provide: %d, should be: 1024 <= port <= 65535\n", port);
        exit(1);
    }
    strcpy(unix_socket_file_path, argv[2]);

    // Preparations
    signal(SIGINT, sigintHandler);
    srand(time(NULL));
    monitor = epoll_create1(0);
    
    // Passive sockets preparing
    localSocketInit();
    netSocketInit();

    // Threads launching
    int resp;
    resp = pthread_create(&listenning_thread, NULL, listenning, NULL);
    if(resp != 0) {perror("ERROR on creating thread"); exit(0); }
    resp = pthread_create(&director_thread, NULL, director, NULL);
    if(resp != 0) {perror("ERROR on creating thread"); exit(0); }
    // resp = pthread_create(&ping_thread, NULL, ping, NULL);
    // if(resp != 0) {perror("ERROR on creating thread"); exit(0); }

    pthread_join(director_thread, NULL);
    
    exit(0);
}