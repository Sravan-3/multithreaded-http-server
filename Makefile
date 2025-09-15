CC = gcc
CFLAGS = -Wall -Wextra -g -I. -I./http-server -I./thread-pooler -I./worker-threads

TARGET = server

SRCS = main.c \
       http-server/http_server.c \
       thread-pooler/thread_pooler.c \
       worker-threads/worker_thread.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Generic rule for building .o from .c (works with subdirs)
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
