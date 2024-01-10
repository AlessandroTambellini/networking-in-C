#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

#define HOSTNAME_SIZE 100

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int main(void)
{
    sockaddr_in serv = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    sockaddr_in dest;
    socklen_t socksize = sizeof(sockaddr_in);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) 
        manageExit("Unable to create socket");

    // bind a socket to a port
    if (bind(sock, (sockaddr *)&serv, sizeof(sockaddr)) == -1) 
        manageExit("Unable to bind");

    // start listening
    if (listen(sock, 1) == -1) 
        manageExit("Unable to listen for new connections");

    printf("%sListening on port %d for incoming requests\n%s", GREEN, PORT, RESET);

    char msg[] = "you connected to the server\n";

    int connect_socket;
    while (1) 
    {
        connect_socket = accept(sock, (sockaddr *)&dest, &socksize);
        
        if (connect_socket == -1) 
            manageExit("Unable to open new socket");

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
