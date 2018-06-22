#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>
#include <signal.h>

#include <netdb.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MESSAGE_TEXTBUFFER_SIZE 256
// #define UNIX_SOCKET_FILE_PATH "./unixSocket.socket" // better shorter than 92 (max size on some unix systems)
#define CLIENT_NAME_MAX_LEN 64
#define MAX_CLIENTS_NUMBER 20
#define HOSTNAME_BUFFER_SIZE 256


//Message types
#define MT_NAME 0
#define MT_JOB 1
#define MT_RESULT 2
#define MT_COMMUNICATE 3
#define MT_PING 4
#define MT_SHUTDOWN 5

typedef struct Message
{
    short int type;
    char text[MESSAGE_TEXTBUFFER_SIZE];
} Message;

int makeMessage(char* text, int type, Message* output);
void printMessage(Message* message);

#endif