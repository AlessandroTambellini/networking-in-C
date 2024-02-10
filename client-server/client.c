#include <netinet/in.h>
#include <string.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void 
startCodingSession(int sock_FD, char req[], char res[]);

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
    ssize_t req_len = REQ_LEN, res_len = 0;
    int ask_again = 0;

    while (1)
    {
        printf("req: ");

        do
        {
            // no problem with buffer overflows because there's always one between '\0' or '\n'
            req[REQ_LEN - 1] = 'j';
            // fgets adds \0 at the end of the string, even if is overflowing
            fgets(req, REQ_LEN, stdin);
            ask_again = req[REQ_LEN - 1] == 'j' ? 0 : 1;
            req[strcspn(req, "\n")] = '\0';
        } while (ask_again);

        // TODO: tell the user the req was too long
        

        // 1) send the req
        req_len = send(client_socket_FD, req, strlen(req) + 1, 0); 
        if (req_len == -1)
            handle_break("Unable to send msg to server");

        // 2) get the res
        res_len = recv(client_socket_FD, res, RES_LEN, 0); 
        if (res_len == -1)
            handle_break("Unable to recevice res from server");

        // 3) act after the res is recv
        if (strcmp(req, GREET) == 0)
            printf(CYAN "res: %s\n" RESET, res);
        else if (strcmp(req, HELP) == 0)
            printf(CYAN "%s\n" RESET, res);
        else if (strcmp(res, REQ_INVALID) == 0)
            printf(RED "res: %s\n" RESET, res);
        else if (strcmp(res, OCS) == 0)
            startCodingSession(client_socket_FD, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
        
        memset(res, 0, RES_LEN);
    }

    close(client_socket_FD);    
    printf("Connection closed.\n");
}

void startCodingSession(int sock_FD, char req[], char res[])
{
    ssize_t req_len = 0, res_len = 0;
    printf("\n[TERMINAL_OPEN]\n");

    while (1)
    {
        printf(YELLOW "\t" BOLD "> ");
        /* if the req len > 255 chars, the exceeding chars are not deleted, but stored for the next req */
        fgets(req, REQ_LEN, stdin);
        req[strcspn(req, "\n")] = 0;

        req_len = send(sock_FD, req, strlen(req) + 1, 0);
        if (req_len == -1)
            handle_break("Unable to send the code line");

        // delete empty line after enter key is pressed
        // printf("\x1b[A");
        
        // the PRINT is handled separately because there can be more chunks receviced, being the possible len of a program grather than RES_LEN (255 chars + \0 == 256)
        if (strcmp(req, PRINT) == 0)
        {
            printf(MAGENTA "\t" "< ");
            while (1)
            {
                res_len = recv(sock_FD, res, RES_LEN, 0);
                if (res_len == -1) {
                    handle_break("Unable to recv res"); 
                } 
                printf("%s", res);
                if (res_len < RES_LEN)
                    break;
            }
            printf("\n");
        }
        else
        {
            res_len = recv(sock_FD, res, RES_LEN, 0);
            if (res_len == -1)
                handle_break("Unable to recv res"); 
        }

        // PRINT is not managed here, but just above
        if (strcmp(res, READ_ERR) == 0)
            printf("Server unable to read code line\n");
        else if (strcmp(res, ADD_ERR) == 0)
            printf(MAGENTA "\t" "< " "Reached max program size: %d\n", PROGRAM_SIZE);
        else if (strcmp(res, EXEC_ERR) == 0)
            printf(MAGENTA "\t" "< " "Program result: ERROR\n");
        else if (strcmp(res, EXEC_OK) == 0)
            printf(MAGENTA "\t" "< " "Program result: RESULT\n");
        else if (strcmp(res, CLEAR_ERR) == 0)
            printf("Server unable to clear program\n");
        else if (strcmp(res, END_OK) == 0)
            break;
    }

    if (strcmp(res, END_OK) == 0)
        printf(MAGENTA "\t" "< " "ok");

    printf(RESET "\n[TERMINAL_CLOSE]\n\n");
}
