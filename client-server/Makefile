CC = clang
CFLAGS = -Wall -Wextra -std=c11

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c common.c server_utils.c -o server.out

client: client.c
	$(CC) $(CFLAGS) client.c common.c client_utils.c -o client.out -lncurses

clean:
	rm *.out
