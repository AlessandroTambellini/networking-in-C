CC = clang
CFLAGS = -Wall -Wextra -std=c11

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c common.c -o server.out

client: client.c
	$(CC) $(CFLAGS) client.c common.c -o client.out

clean:
	rm *.out
