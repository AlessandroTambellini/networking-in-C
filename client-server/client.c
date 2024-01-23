#include <netinet/in.h>
#include <string.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void startCodingSession(int sock_FD, char req[], char res[]);

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
        handle_exit("Unable to connect socket");

    printf(CYAN "Connected to server on port %d successfully\n" RESET, PORT);
    printf("Type HELP in the req to know the commands\n\n");

    char res[RES_LEN]; 
    char req[REQ_LEN];
  
    while (1)
    {
        // 1) write the req
        printf("req: ");
        // no problem with buffer overflows because there's always one between '\0' or '\n'
        fgets(req, REQ_LEN, stdin); 
        req[strcspn(req, "\n")] = 0;
        // 

        // 2) send the req
        int isReqSent = send(client_socket_FD, req, strlen(req) + 1, 0); 
        if (isReqSent == -1)
            handle_break("Unable to send msg to server");

        // 3) get the res
        int isResRecv = recv(client_socket_FD, res, RES_LEN, 0); 
        if (isResRecv == -1)
            handle_break("Unable to recevice res from server");
        printf(CYAN "res: %s\n" RESET, res); 

        // 3) act after the res is recv
        if (strcmp(res, OCS) == 0)
        {
            printf("Opening coding session...\n");
            startCodingSession(client_socket_FD, req, res);
        }
        else if (strcmp(res, CLOSED) == 0)
        {
            printf("Connection closed.\n");
            break;
        }
    }

    close(client_socket_FD);    
}

void startCodingSession(int sock_FD, char req[], char res[])
{
    printf(YELLOW " " BOLD "\n");
    while (1) 
    {
        printf("> ");
        fgets(req, REQ_LEN, stdin);
        req[strcspn(req, "\n")] = 0;

        int isReqSend = send(sock_FD, req, strlen(req) + 1, 0);
        if (isReqSend == -1)
            handle_break("Unable to send the code line");
        // 
        // int isResRecv = recv(sock_FD, res, strlen(res), 0);
        // if (isResRecv == -1)
        //     handle_break("Unable to recv res");
        // printf("CS res: %s", res);
    }
    printf(RESET);
}
