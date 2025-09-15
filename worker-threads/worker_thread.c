#include "worker_thread.h"

static int send_all(int sockfd, const void *buf, size_t len) {
    size_t total_sent = 0;
    const char *p = buf;

    while (total_sent < len) {
        ssize_t n = send(sockfd, p + total_sent, len - total_sent, 0);
        if (n <= 0) {
            return -1; 
        }
        total_sent += n;
    }
    return 0;
}

void serve_static_pages(void *client_fd_p) {
    int client_fd = *(int *) client_fd_p;
    free(client_fd_p);

    char buffer[CLIENT_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int valread = read(client_fd, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Client says:\n%s\n", buffer);
    } else {
        printf("Client sent no data\n");
    }

    char path[512];
    snprintf(path, sizeof(path),
             "%s/mt-http-server/static-pages/index.html",
             getenv("HOME"));

    int static_page_fd = open(path, O_RDONLY);
    if (static_page_fd < 0) {
        perror("open failed");
        close(client_fd);
        return;
    }

    struct stat st;
    if (fstat(static_page_fd, &st) < 0) {
        perror("fstat failed");
        close(static_page_fd);
        close(client_fd);
        return;
    }

    size_t file_size = st.st_size;
    char *file_buf = malloc(file_size);
    if (!file_buf) {
        perror("malloc failed");
        close(static_page_fd);
        close(client_fd);
        return;
    }

    ssize_t bytes_read = read(static_page_fd, file_buf, file_size);
    if (bytes_read < 0) {
        perror("read failed");
        free(file_buf);
        close(static_page_fd);
        close(client_fd);
        return;
    }
    close(static_page_fd);

    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n"
             "\r\n",
             (long) file_size);

    if (send_all(client_fd, header, strlen(header)) < 0) {
        perror("send header failed");
    } else if (send_all(client_fd, file_buf, file_size) < 0) {
        perror("send body failed");
    }

    free(file_buf);
    close(client_fd);
}
