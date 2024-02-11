#include <netinet/in.h>
#include <string.h>
#include <stdio_ext.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void
showStartMsg(void);
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

    showStartMsg();
    
    char res[RES_LEN], req[REQ_LEN];
    ssize_t req_len = REQ_LEN, res_len = 0;
    bool isReqTooLong = false, isReqEmpty = false;

    while (1)
    {
        do
        {
            printf("req: ");
            
            // just a symbol different than \0. If the string is too long is substituted with \0 by fgets
            req[REQ_LEN - 1] = 'j';

            // fgets adds \0 at the end of the string, even if is overflowing. Plus, when you enter, you add '\n'. Therefore it is string + '\n' + '\0'. So, max input is 253 chars
            fgets(req, REQ_LEN, stdin);
            // discard all pending chars in the stream if the req was > 255
            __fpurge(stdin);

            isReqTooLong = req[REQ_LEN - 1] == 'j' ? false : true;
            if (isReqTooLong)
                printf(RED BOLD "The req is too long!\n" RESET);
            isReqEmpty = strcmp(req, "\n") == 0 ? true : false;

            // '\n' is read by the server and if so is not able to understand the req
            req[strcspn(req, "\n")] = '\0';

        } while (isReqTooLong || isReqEmpty);

        // exceptional req not sent to server
        if (strcmp(req, CLEAR) == 0)
        {
            system("clear");
            showStartMsg();
            // I do not want to nest if-else for a single case
            continue;
        }

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

        fgets(req, REQ_LEN, stdin);
        __fpurge(stdin);
        req[strcspn(req, "\n")] = '\0';

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

void showStartMsg(void)
{
    printf(CYAN "Connected to server on port %d successfully\n" RESET, PORT);
    printf("Type HELP in the req to know the commands\n\n");
}
