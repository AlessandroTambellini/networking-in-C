#include <arpa/inet.h>

#include "server_utils.h"

#define HOSTNAME_SIZE 100
#define LISTEN_BACKLOG 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int 
main(void)
{
    sockaddr_in server_addr = {0}, client_addr = {0};

    server_addr = (sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    socklen_t client_addr_size = sizeof(sockaddr_in);

    int opt = 1;
    int server_socket_FD = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_FD == -1) 
        handle_exit("Unable to create socket");

    // Set socket option to allow address reuse --> avoid "Unable to bind : Address already in use" error
    if (setsockopt(server_socket_FD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        handle_exit("Unable to reuse address");
    }

    if (bind(server_socket_FD, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1) 
        handle_exit("Unable to bind");

    if (listen(server_socket_FD, LISTEN_BACKLOG) == -1) 
        handle_exit("Unable to listen for new connections");

    printf(GREEN "Listening on port %d for incoming requests\n" RESET, PORT);

    int client_socket_FD = accept(server_socket_FD, (sockaddr *)&client_addr, &client_addr_size);
    
    if (client_socket_FD == -1) 
        handle_exit("Unable to open new socket");
    printf("Client connected from IP %s from port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char req[REQ_LEN], res[RES_LEN];
    ssize_t req_len = 0;
    while (1)
    {   
        req_len = read(client_socket_FD, req, REQ_LEN);
        if (req_len == -1)
            handle_break("Unable to read client req");

        printf(CYAN "req: %s\n" RESET, req);

        // 1) write the res
        if (strcmp(req, GREET) == 0)
            strcpy(res, "Hello!");
        else if (strcmp(req, CODE) == 0)
            openCodingSession(client_socket_FD, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
        else if (strcmp(req, HELP) == 0)
            getHelpTxt(res);
        else
            strcpy(res, REQ_INVALID);

        // 2) send the res
        sendRes(client_socket_FD, res, strlen(res) + 1);
    }

    // close(client_socket_FD);
    close(server_socket_FD);
    printf("Connection closed.\n");
}
