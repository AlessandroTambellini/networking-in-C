#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000
#define HOSTNAME_SIZE 100

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int main(void)
{
    sockaddr_in dest; // socket info about the machine connecting to us
     // socket info about our server
    socklen_t socksize = sizeof(sockaddr_in);

    // address setup
    sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) 
    {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    // bind a socket to a port
    if (bind(sock, (sockaddr *)&serv, sizeof(sockaddr)) == -1) 
    {
        perror("Error: unable to bind");
        exit(EXIT_FAILURE);
    }

    char hostname[HOSTNAME_SIZE];
    gethostname(hostname, HOSTNAME_SIZE);
    printf("hostname: %s\n", hostname);

    // start listening
    if (listen(sock, 1) == -1) 
    {
        perror("Error: unable to listen for new connections");
        exit(EXIT_FAILURE);
    }  

    char msg[] = "you connected to the server\n";

    int connect_socket;
    while (1) 
    {
        connect_socket = accept(sock, (sockaddr *)&dest, &socksize);
        if (connect_socket == -1) 
        {
            perror("Error: unable to open new socket");
            exit(EXIT_FAILURE);
        }
        printf("Incoming connection from %s - sending msg\n", inet_ntoa(dest.sin_addr));
        int res = send(connect_socket, msg, strlen(msg), 0);
        if (res == -1)
        {
            printf("Unable to send msg to %s\n", inet_ntoa(dest.sin_addr));
            close(connect_socket);
        }
        else
            printf("Message sent to %s\n", inet_ntoa(dest.sin_addr));
    }

    close(sock);
}
