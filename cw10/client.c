#include "common.h"

int socketFD;
char name[CLIENT_NAME_MAX_LEN];
char mode[10];
char hostname[HOSTNAME_BUFFER_SIZE];
int port;
char unix_socket_file_path[91];

void sigintHandler(int signum)
{
    printf("\nSIGINT received\n" );

    // Shutdown message sending
    Message buffer;
    makeMessage("shutdown", MT_SHUTDOWN, &buffer);
    int n = write(socketFD, &buffer, sizeof(buffer));
    if(n < 0) { perror("ERROR writing to socket"); exit(0); }

    // Disconnection
    shutdown(socketFD, SHUT_WR);
    exit(0);
}

void calculate(Message* input, Message* output)
{
    if(input->type != MT_JOB)
    {
        printf("ERROR on calculation: invalid input message type\n");
        raise(SIGINT);
    }
    if(output == NULL)
    {
        printf("ERROR on calculation: output message pointer is NULL\n");
        raise(SIGINT);
    }

    long long calculationNumber;
    float arg1;
    char operator;
    float arg2;
    sscanf(input->text, "%lld: %f %c %f", &calculationNumber, &arg1, &operator, &arg2);
    float result;
    if(operator == '+') result = arg1 + arg2;
    else if(operator == '-') result = arg1 - arg2;
    else if(operator == '*') result = arg1 * arg2;
    else if(operator == '/') result = arg1 / arg2;
    else { makeMessage("Invalid calculation\n", MT_RESULT, output); return; }

    char textbuffer[MESSAGE_TEXTBUFFER_SIZE];
    sprintf(textbuffer, "%lld: %f (by client: '%s')", calculationNumber, result, name);

    makeMessage(textbuffer, MT_RESULT, output);
}

void establishConnection()
{
    if(strcmp(mode,"local") == 0) // LOCAL VERSION
    {
        // Socket creating
        socketFD = socket(AF_UNIX, SOCK_STREAM, 0);
        if(socketFD < 0) { perror("ERROR opening socket"); exit(1); }

        // Server address initialization
        struct sockaddr_un serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sun_family = AF_UNIX;
        strcpy(serverAddress.sun_path, unix_socket_file_path);

        // Connecting to the server
        int connectResponse = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if( connectResponse < 0) { perror("ERROR connecting"); exit(1); }
    }
    else if(strcmp(mode, "net") == 0) // NET VERSION
    {
        // Server host getting
        struct hostent* server = gethostbyname(hostname);
        if(server == NULL) { fprintf(stderr,"ERROR, no such host\n"); exit(1); }

        // Socket creating
        socketFD = socket(AF_INET, SOCK_STREAM, 0);
        if(socketFD < 0) { perror("ERROR opening socket"); exit(1); }

        // Server address initialization
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        memmove(&serverAddress.sin_addr.s_addr, server->h_addr, server->h_length);
        serverAddress.sin_port = htons(port);

        // Connecting to the server
        int connectResponse = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if( connectResponse < 0) { perror("ERROR connecting"); exit(1); }
    }
    else { printf("Invalid mode, provided: %s, should be 'local' or 'net'\n", mode); exit(1);}
}

int main(int argc, char** argv)
{
    // Arguments parsing
    if(argc == 4)
    {
        strcpy(name, argv[1]);
        strcpy(mode, argv[2]);
        if(!(strcmp(mode, "local") == 0))
        {
            fprintf(stderr,"Usage: %s name 'net' hostname port OR %s name 'local' unixSocketFile \n", argv[0], argv[0]);
            exit(1);
        }
        strcpy(unix_socket_file_path, argv[3]);
        printf("name: %s, mode: %s, unixSocketFile: %s\n", name, mode, unix_socket_file_path);
    }
    else if(argc == 5)
    {
        strcpy(name, argv[1]);
        strcpy(mode, argv[2]);
        if(!(strcmp(mode, "net") == 0))
        {
            fprintf(stderr,"Usage: %s name 'net' hostname port OR %s name 'local' unixSocketFile \n", argv[0], argv[0]);
            exit(1);
        }
        strcpy(hostname, argv[3]);
        port = strtol(argv[4], NULL, 10);
        printf("name: %s, mode: %s, hostname: %s, port: %d\n", name, mode, hostname, port);
    }
    else
    {
        fprintf(stderr,"Usage: %s name 'net' hostname port OR %s name 'local' unixSocketFile \n", argv[0], argv[0]);
        exit(1);
    }

    // Connect
    establishConnection();

    // Signal handling
    signal(SIGINT, sigintHandler);

    // COMMUNICATION
    Message message;
    int n;

    // Send name
    int resp = makeMessage(name, MT_NAME, &message);
    if(resp == -1) printf("ERROR on message making");
    n = write(socketFD, &message, sizeof(message));
    if(n < 0) { perror("ERROR writing to socket"); raise(SIGINT); }

    // Receive confirmation of registration from the server
    n = read(socketFD, &message, sizeof(message));
    if(n < 0) { perror("ERROR reading from socket"); raise(SIGINT); }
    printf("%s\n", message.text);
    if(message.type==MT_SHUTDOWN)
    {
        shutdown(socketFD, SHUT_RD);
        exit(0);
    }

    Message result;
    while(1)
    {
        // Receive job from the server
        n = read(socketFD, &message, sizeof(message));
        if(n < 0) { perror("ERROR reading from socket"); raise(SIGINT); }

        if(message.type == MT_JOB)
        {
            printf("%s\n", message.text);
            calculate(&message, &result);
            
            // Send message to the server
            n = write(socketFD, &result, sizeof(message));
            if(n < 0) { perror("ERROR writing to socket"); raise(SIGINT); }
        }
        else if(message.type==MT_SHUTDOWN)
        {
            shutdown(socketFD, SHUT_RD);
            printf("Server has exited\n");
            exit(0);
        }
        else if(message.type==MT_PING)
        {
            printf("PING\n");

            // Response
            Message response;
            makeMessage("PING RESPONSE", MT_PING, &response);
            int n = write(socketFD, &response, sizeof(response));
            if(n < 0) { perror("ERROR writing to socket"); exit(1); }
        }
        else printMessage(&message);
    }

    exit(0);
}