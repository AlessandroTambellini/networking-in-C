#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

#define HOSTNAME_SIZE 100

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int 
main(void)
{
    // socketaddr_in vs socketaddr_un
    sockaddr_in server_addr, client;
    // avoid garbage values
    memset(&server_addr, 0, sizeof(server_addr)); 
    memset(&client, 0, sizeof(client));

    server_addr = (sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    socklen_t socksize = sizeof(sockaddr_in);

    int server_socket_FD= socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_FD== -1) 
        handle_error("Unable to create socket");

    // bind a socket to a port
    if (bind(server_socket_FD, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1) 
        handle_error("Unable to bind");

    // start listening
    if (listen(server_socket_FD, 1) == -1) 
        handle_error("Unable to listen for new connections");

    printf("%sListening on port %d for incoming requests\n%s", GREEN, PORT, RESET);

    char req[REQ_LEN];
    char res[RES_LEN] = "random response";

    // just 1 client can connect 
    int client_socket_FD = accept(server_socket_FD, (sockaddr *)&client, &socksize);
    
    if (client_socket_FD == -1) 
        handle_error("Unable to open new socket");
    printf("Client connected from IP %s on port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    while (1) 
    {   
        read(client_socket_FD, req, sizeof(req));
        printf("%sreq: %s%s\n", CYAN, req, RESET);

        int isMsgSent = 0;
        isMsgSent = send(client_socket_FD, res, strlen(res), 0);
        if (isMsgSent == -1)
        {
            printf("Unable to send res to %s\n", inet_ntoa(client.sin_addr));
            close(client_socket_FD);
        }
        else
        {
            printf("%sres sent: ok\n%s", GREEN, RESET);
        }
    }

    close(client_socket_FD);
    close(server_socket_FD);
}
