#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORTNUM 3000

typedef struct sockaddr sockaddr;

#define MAX_HOSTNAME_SIZE 100

int main(int argc, char *argv[])
{
    struct sockaddr_in dest; // socket info about the machine connecting to us
    struct sockaddr_in serv; // socket info about our server
    socklen_t socksize = sizeof(struct sockaddr_in);

    // address setup
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(PORTNUM);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) 
    {
        switch (errno) 
        {
        case EPROTONOSUPPORT:
            printf("Error: protocol not supported.\n");
            break;
        case EACCES:
            printf("Error: permission denied.\n");
            break;
        default:
            printf("Error: unknown error in socket function.\n");
            break;
        }
        exit(EXIT_FAILURE);
    }

    // bind a socket to a port
    if (bind(sock, (sockaddr *)&serv, sizeof(sockaddr)) == -1) 
    {
        printf("Error: unable to bind.\n");
        exit(EXIT_FAILURE);
    }

    char hostname[MAX_HOSTNAME_SIZE];
    gethostname(hostname, MAX_HOSTNAME_SIZE);
    printf("hostname: %s\n", hostname);

    // start listening
    if (listen(sock, 1) == -1) 
    {
        printf("Error: unable to listen for new connections.\n");
        exit(EXIT_FAILURE);
    }

    char msg[] = "you connected to the server\n";

    int connect_socket;
    while (1) 
    {
        connect_socket = accept(sock, (sockaddr *)&dest, &socksize);
        if (connect_socket == -1) 
        {
            printf("Error: unable to open new socket.\n");
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
    return EXIT_SUCCESS;
}
