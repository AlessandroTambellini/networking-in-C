#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

#define HOSTNAME_SIZE 100
#define LISTEN_BACKLOG 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void readCode(int sock_FD, char req[], char res[]);

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

    int server_socket_FD = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_FD == -1) 
        handle_exit("Unable to create socket");

    if (bind(server_socket_FD, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1) 
        handle_exit("Unable to bind");

    // start listening, and just 1 client can connect
    if (listen(server_socket_FD, LISTEN_BACKLOG) == -1) 
        handle_exit("Unable to listen for new connections");

    printf("%sListening on port %d for incoming requests\n%s", GREEN, PORT, RESET);

    char req[REQ_LEN];
    char res[RES_LEN];

    int client_socket_FD = accept(server_socket_FD, (sockaddr *)&client_addr, &client_addr_size);
    
    if (client_socket_FD == -1) 
        handle_exit("Unable to open new socket");
    printf("Client connected from IP %s from port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1)
    {   
        int isReqRead = read(client_socket_FD, req, sizeof(req));
        if (!isReqRead)
            handle_break("Unable to read client req");
        printf("%sreq: %s%s\n", CYAN, req, RESET);

        // 1) write the res
        if (strcmp(req, GREET) == 0)
            strcpy(res, "Hello!");
        else if (strcmp(req, CODE) == 0)
            strcpy(res, OCS);
        else if (strcmp(req, CLOSE) == 0)
            strcpy(res, CLOSED);
        else
            strcpy(res, "not a valid req");

        // 2) send the res
        int isMsgSent = send(client_socket_FD, res, strlen(res) + 1, 0);
        if (isMsgSent == -1)
            handle_break("Unable to send res to client");
        printf("%sres sent: ok\n%s", GREEN, RESET);
        
        // 3) act after the res is send 
        if (strcmp(res, CLOSED) == 0)
            break;
        else if (strcmp(res, OCS) == 0)
            printf("Listening for incoming code\n");
    }

    close(client_socket_FD);
    close(server_socket_FD);
}

void readCode(int sock_FD, char req[], char res[])
{
    char program[PROGRAM_SIZE] = {0};

    strcpy(res, "OCS");
    send(sock_FD, res, strlen(res) + 1, 0);

    while (1)
    {
        int isReqRead = read(sock_FD, req, strlen(req));
        if (!isReqRead)
        {
            strcpy(res, "Unable to read the req");
            send(sock_FD, res, strlen(res) + 1, 0);
        }
        else if (strcmp(req, "END"))
            break;
        else if (strlen(program) + strlen(req) > PROGRAM_SIZE)
        {
            strcpy(res, "Reached max dimension of the program");
            send(sock_FD, res, strlen(res) + 1, 0);
            break;
        }
        else
            strcat(program, req);
    }

    printf("program: %s\n", program);
}
