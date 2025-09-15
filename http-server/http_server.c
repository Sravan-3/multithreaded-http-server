#include "http_server.h"
#include "thread_pooler.h"
#include "worker_thread.h"

#define QUEUE_SIZE 32

int run_server() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on http://127.0.0.1:%d\nOr access it via your interface IP (use 'ip addr show') on port %d\n", PORT, PORT);


    thread_pool_t pool;
    thread_pool_init(&pool, QUEUE_SIZE);

    while (1) {
    
        new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (new_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        int *c_fd = malloc(sizeof(int));

        *c_fd = new_socket;

        thread_pool_add(&pool, serve_static_pages, (void *) c_fd);
    }

    thread_pool_destroy(&pool);
    close(server_fd);
    return 0;
}
