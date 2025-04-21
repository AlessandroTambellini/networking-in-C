#include <stdio_ext.h>

#include "common.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void open_coding_session(int sock);

int 
main(void) 
{
    int client_sock = socket(AF_INET, SOCK_STREAM, 0); 
  
    sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server = (sockaddr_in) {
        .sin_family = AF_INET, 
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY, 
    };  

    printf("Connecting to the server...\n");

    if (connect(client_sock, (sockaddr*)&server, sizeof(server)) == -1) 
        handle_exit("Unable to connect socket.");

    printf(CYAN "Connected on port %d.\n" RESET, PORT);
    printf("Type \"help\" to list the commands.\n");
    
    while (1)
    {
        char req[MAX_REQ_LEN] = { 0 };
        bool req_empty = false;
        
        do {
            printf("["BOLD"user"RESET"@machine ~]$ ");
            
            strcpy(req, fgets(req, MAX_REQ_LEN, stdin));
            // discard all pending chars in the stream if the req is greather than MAX_REQ_LEN
            __fpurge(stdin);
            
            req_empty = strcmp(req, "\n") == 0 ? true : false;
            
            // for now there're no commands that need more than a single word. So I can simply substitute empty spaces with end of string char.
            req[strcspn(req, " ")] = '\0';
            req[strcspn(req, "\n")] = '\0';
            
        } while (req_empty);
        
        // The only req not processed by the server
        if (strcmp(req, CLEAR) == 0) {
            system("clear");
            continue;
        }
        
        // 1) send the req
        if (send(client_sock, req, strlen(req) + 1, 0) == -1)
        handle_break("Unable to send msg to server");
        
        // 2) get the res
        char res[MAX_RES_LEN] = { 0 };
        if (recv(client_sock, res, MAX_RES_LEN, 0) == -1)
            handle_break("Unable to recevice res from server");

        // 3) act after the res is recv
        if (strcmp(req, GREET) == 0)
            printf(CYAN "%s\n" RESET, res);
        else if (strcmp(req, HELP) == 0)
            printf(CYAN "%s\n" RESET, res);
        else if (strcmp(res, REQ_INVALID) == 0)
            printf(RED "%s\n" RESET, res);
        else if (strcmp(res, OCS) == 0)
            open_coding_session(client_sock);
        else if (strcmp(req, CLOSE) == 0)
            break;
        else {
            printf("%s\n", res);
        }        
    }

    close(client_sock);    
    printf("Connection closed.\n");
}

void open_coding_session(int sock)
{
    printf("psychic-disco 0.1.0\n");

    while (1)
    {
        printf(YELLOW BOLD ">>> ");

        char cmd[MAX_REQ_LEN] = { 0 };
        char res[MAX_RES_LEN] = { 0 };

        fgets(cmd, MAX_REQ_LEN, stdin);
        __fpurge(stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (send(sock, cmd, strlen(cmd) + 1, 0) == -1)
            handle_break("Unable to send the code line");

        // the PRINT is handled separately because there can be more chunks receviced, being the possible len of a program grather than MAX_RES_LEN
        if (strcmp(cmd, PRINT) == 0)
        {
            printf(MAGENTA);
            while (1)
            {
                ssize_t res_len = recv(sock, res, MAX_RES_LEN, 0);
                if (res_len == -1)
                    handle_break("Unable to recv res"); 
                printf("%s", res);
                if (res_len < MAX_RES_LEN)
                    break;
            }
        }
        else
        {
            if (recv(sock, res, MAX_RES_LEN, 0) == -1)
                handle_break("Unable to recv res"); 
        }

        // PRINT is not managed here, but above
        if (strcmp(res, READ_ERR) == 0)
            printf("Server unable to read code line\n");
        else if (strcmp(res, ADD_ERR) == 0)
            printf(MAGENTA "Reached max program size: %d\n", PROGRAM_SIZE);
        else if (strcmp(res, EXEC_ERR) == 0)
            printf(MAGENTA "Program result: ERROR\n");
        else if (strcmp(res, EXEC_OK) == 0)
            printf(MAGENTA "Program executed.\n");
        else if (strcmp(res, CLEAR_ERR) == 0)
            printf("Server unable to clear program\n");
        else if (strcmp(res, END_OK) == 0)
            break;
    }

    printf(RESET);
}
