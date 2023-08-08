#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORTNUM 2300
#define MAX_HOSTNAME_SIZE 100

int mistring()
{
    return 1;
}

int main(int argc, char *argv[])
{
    char *msg = "Hello World !\n";

    struct sockaddr_in dest; // socket info about the machine connecting to us
    struct sockaddr_in serv; // socket info about our server
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0,
        sizeof(serv)); // zero the struct before filling the fields
    serv.sin_family = AF_INET; // set the type of connection to TCP/IP
    serv.sin_addr.s_addr = htonl(INADDR_ANY); // set our address to any interface
    serv.sin_port = htons(PORTNUM); // set the server port number

    int mysocket = socket(AF_INET, SOCK_STREAM, 0); // socket(family, type, protocol)
    /*
        family
            AF_INET for IPv4
            AF_INET6 for IPv6
        type
            SOCK_STREAM for TCP
            SOCK_DGRM for UDP
        protocol
            (not used for internet sockets)
    */
    if (mysocket == -1) {
        switch (errno) {
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
    if (bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == -1) {
        printf("Error: unable to bind.\n");
        exit(EXIT_FAILURE);
    }

    char hostname[MAX_HOSTNAME_SIZE];
    gethostname(hostname, MAX_HOSTNAME_SIZE);
    printf("hostname: %s\n", hostname);

    /* start listening, listen(socket, backlog). backlog = max num of connections
    the socket will queue up, each waiting to be accepted */
    if (listen(mysocket, 1) == -1) {
        printf("Error: unable to listen for new connections.\n");
        exit(EXIT_FAILURE);
    }

    int connect_socket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
    if (connect_socket == -1) {
        printf("Error: unable to open new socket.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Incoming connection from %s - sending msg\n", inet_ntoa(dest.sin_addr));
        send(connect_socket, msg, strlen(msg), 0);
        close(connect_socket);
        connect_socket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
    }

    close(mysocket);
    return EXIT_SUCCESS;
}

/* NOTES
- sockaddr and sockadd_in have the same size, but they interpret the bits in a
different way
- in the networking there's an abuse of casting for addresses and ports.
Instead could have been used union
 */