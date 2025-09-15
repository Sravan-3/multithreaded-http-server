#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define CLIENT_BUFFER_SIZE 1024

int run_server();

#endif