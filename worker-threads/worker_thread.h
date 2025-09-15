#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h> 


#define CLIENT_BUFFER_SIZE 1024

void serve_static_pages(void *client_fd_p);

#endif