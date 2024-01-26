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

    char res[RES_LEN], req[REQ_LEN];
    int isReqSent = 0, isResRecv = 0;

    while (1)
    {
        printf("req: ");
        // no problem with buffer overflows because there's always one between '\0' or '\n'
        fgets(req, REQ_LEN, stdin); 
        req[strcspn(req, "\n")] = 0;
        // 

        // 1) send the req
        isReqSent = send(client_socket_FD, req, strlen(req) + 1, 0); 
        if (isReqSent == -1)
            handle_break("Unable to send msg to server");

        // 2) get the res
        isResRecv = recv(client_socket_FD, res, RES_LEN, 0); 
        if (isResRecv == -1)
            handle_break("Unable to recevice res from server");

        // 3) act after the res is recv
        if (strcmp(req, GREET) == 0)
            printf(CYAN "res: %s\n" RESET, res);
        else if (strcmp(res, REQ_INVALID) == 0)
            printf(RED "res: %s\n" RESET, res);
        else if (strcmp(res, OCS) == 0)
            startCodingSession(client_socket_FD, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
    }

    close(client_socket_FD);    
    printf("Connection closed.\n");
}

void startCodingSession(int sock_FD, char req[], char res[])
{
    printf("\n[TERMINAL_OPEN]\n");

    while (1)
    {
        printf(YELLOW "\t" BOLD "> ");
        fgets(req, REQ_LEN, stdin);
        req[strcspn(req, "\n")] = 0;

        int isReqSent = send(sock_FD, req, strlen(req) + 1, 0);
        if (isReqSent == -1)
            handle_break("Unable to send the code line");

        int isResRecv = recv(sock_FD, res, RES_LEN, 0);
        if (isResRecv == -1)
            handle_break("Unable to recv res");
        
        // just for debugging for now
        printf(MAGENTA "\t" "< ");
        if (strcmp(res, READ_ERR) == 0)
            printf("Server unable to read code line");
        else if (strcmp(res, ADD_ERR) == 0)
            printf("Reached max program size: %d", PROGRAM_SIZE);
        else if (strcmp(res, EXEC_ERR) == 0)
            printf("Program result: ERROR");
        else if (strcmp(res, EXEC_OK) == 0)
            printf("Program result: RESULT");
        else if (strcmp(res, CLEAR_ERR) == 0)
            printf("Server unable to clear program");
        else if (strcmp(req, PRINT) == 0)
            printf("%s", res);
        else if (strcmp(res, END_OK) == 0)
            break;
        else
            printf("ok");
        printf("\n");
    }

    if (strcmp(res, END_OK) == 0)
        printf("ok");

    printf(RESET "\n[TERMINAL_CLOSE]\n\n");
}
