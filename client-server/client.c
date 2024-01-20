#include <netinet/in.h>
#include <string.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void startCodingSession(int sock_FD);

int 
main(void) 
{
    int client_socket_FD = socket(AF_INET, SOCK_STREAM, 0); 
  
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr = (sockaddr_in) {
        .sin_family = AF_INET, 
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY, 
    };  

    int isConnected = connect(client_socket_FD, (sockaddr*)&server_addr, sizeof(server_addr)); 
    if (isConnected == -1) 
        handle_error("Unable to connect socket");

    printf("%sConnected to server on port %d successfully\n%s", CYAN, PORT, RESET);

    char res[RES_LEN]; 
    char req[REQ_LEN];
  
    while (1)
    {
        printf("req: ");
        scanf("%s", req);

        // 1) manage the req

        // 2) send the req
        int isReqSent = send(client_socket_FD, req, strlen(req) + 1, 0); 
        if (isReqSent == -1)
            handle_error("Unable to send msg to server");

        int isResRecv = recv(client_socket_FD, res, sizeof(res), 0); 
        if (isResRecv == -1)
            handle_error("Unable to recevice res from server");

        printf("%sres: %s\n%s", CYAN, res, RESET); 

        // 3) manage the res
        if (strcmp(res, OCS) == 0)
        {
            printf("Opening coding session...\n");
            // startCodingSession(client_socket_FD);
        }
        else if (strcmp(res, CLOSED) == 0)
        {
            printf("Connection closed.\n");
            break;
        }
    }

    close(client_socket_FD);    
}

void startCodingSession(int sock_FD)
{
    char codeLine[REQ_LEN];
    char res[RES_LEN];
    while (1)
    {
        printf("\n> ");
        scanf("%s", codeLine);
        int isCodeLineSent = send(sock_FD, codeLine, strlen(codeLine) + 1, 0);
        if (!isCodeLineSent)
            printf("Unable to send the code line\nPlease, try again");
        
        int isResRecv = recv(sock_FD, res, sizeof(res), 0);
        printf("CS res: %s\n", res);
    }
}
