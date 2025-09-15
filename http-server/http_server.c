#include "http_server.h"
#include "thread_pooler.h"
#include "worker_thread.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

#define QUEUE_SIZE 32

int server_fd;

static volatile sig_atomic_t stop_server = 0;


void handle_sigint(int sig) {
    (void)sig;
    stop_server = 1;
    shutdown(server_fd, SHUT_RDWR);
}

int run_server() {
    int new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    
    signal(SIGINT, handle_sigint);

    
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on http://127.0.0.1:%d\n", PORT);

    thread_pool_t pool;
    thread_pool_init(&pool, QUEUE_SIZE);


    while (!stop_server) {
        new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (new_socket < 0) {
            if (stop_server) break; 
            perror("Accept failed");
            continue;
        }

        int *c_fd = malloc(sizeof(int));
        if (!c_fd) {
            perror("malloc failed");
            close(new_socket);
            continue;
        }

        *c_fd = new_socket;

        if (thread_pool_add(&pool, serve_static_pages, (void *)c_fd) != 0) {
            fprintf(stderr, "Failed to add task to thread pool\n");
            free(c_fd); 
            close(new_socket); 
            continue;
        }
    }

    printf("\nShutting down server...\n");
    thread_pool_destroy(&pool);
    close(server_fd);
    return 0;
}