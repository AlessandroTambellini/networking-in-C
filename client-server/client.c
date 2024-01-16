#include <netinet/in.h>
#include <string.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

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

        // + 1: to also send \0
        int isReqSent = send(client_socket_FD, req, strlen(req) + 1, 0); 
        if (isReqSent == -1)
            handle_error("Unable to send msg to server");

        int isResRecv = recv(client_socket_FD, res, sizeof(res), 0); 
        if (isResRecv == -1)
            handle_error("Unable to recevice res from server");

        printf("%sres: %s\n%s", CYAN, res, RESET); 
        
        if (strcmp(res, "Bye!") == 0)
        {
            printf("Connection closed.\n");
            break;
        }
    }

    close(client_socket_FD);    
}
