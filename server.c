#include "common.h"
#include "html.h"

#define MAX_HOSTNAME_SIZE 100

int mistring()
{
    return 1;
}

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

    int mysocket = socket(AF_INET, SOCK_STREAM, 0);
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
    if (bind(mysocket, (sockaddr *)&serv, sizeof(sockaddr)) == -1) {
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

    int connect_socket = accept(mysocket, (sockaddr *)&dest, &socksize);
    if (connect_socket == -1) {
        printf("Error: unable to open new socket.\n");
        exit(EXIT_FAILURE);
    }

    char HTML[HTML_SIZE];
    create_HTML_content(HTML);

    while (1) {
        printf("Incoming connection from %s - sending msg\n", inet_ntoa(dest.sin_addr));
        send(connect_socket, HTML, strlen(HTML), 0);
        close(connect_socket);
        connect_socket = accept(mysocket, (sockaddr *)&dest, &socksize);
    }

    close(mysocket);
    return EXIT_SUCCESS;
}