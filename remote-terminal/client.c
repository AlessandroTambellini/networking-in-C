#include <stdio_ext.h>

#include "common.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void open_coding_session(int sock_FD, char req[], char res[]);

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

    int connected = connect(client_sock, (sockaddr*)&server, sizeof(server)); 
    if (connected == -1) 
        handle_exit("Unable to connect socket.");

    printf(CYAN "Connected on port %d.\n" RESET, PORT);
    printf("Type \"HELP\" to list the commands.\n");
    
    char res[RES_LEN];
    char req[REQ_LEN];
    ssize_t req_len = REQ_LEN;
    ssize_t res_len = 0;
    bool isReqTooLong = false;
    bool isReqEmpty = false;
    while (1)
    {
        do {
            printf("["BOLD"user"RESET"@machine ~]$ ");
            
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


            // for now there're no commands that need more than a single word. So I can simply substitute empty spaces with end of string char.
            req[strcspn(req, " ")] = '\0';
            // '\n' is read by the server and if so is not able to understand the req
            req[strcspn(req, "\n")] = '\0';

        } while (isReqTooLong || isReqEmpty);

        // The only req not processed by the server
        if (strcmp(req, CLEAR) == 0) {
            system("clear");
            continue;
        }

        // 1) send the req
        req_len = send(client_sock, req, strlen(req) + 1, 0); 
        if (req_len == -1)
            handle_break("Unable to send msg to server");

        // 2) get the res
        res_len = recv(client_sock, res, RES_LEN, 0); 
        if (res_len == -1)
            handle_break("Unable to recevice res from server");

        // 3) act after the res is recv
        if (strcmp(req, GREET) == 0)
            printf(CYAN "%s\n" RESET, res);
        else if (strcmp(req, HELP) == 0)
            printf(CYAN "%s\n" RESET, res);
        else if (strcmp(res, REQ_INVALID) == 0)
            printf(RED "%s\n" RESET, res);
        else if (strcmp(res, OCS) == 0)
            open_coding_session(client_sock, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
        else {
            printf("%s\n", res);
        }
        
        memset(res, 0, RES_LEN);
    }

    close(client_sock);    
    printf("Connection closed.\n");
}

void open_coding_session(int sock_FD, char req[], char res[])
{
    ssize_t req_len = 0, res_len = 0;
    printf("psychic-disco 0.1.0\n");

    while (1)
    {
        printf(YELLOW BOLD ">>> ");

        fgets(req, REQ_LEN, stdin);
        __fpurge(stdin);
        // req[strcspn(req, " ")] = '\0';
        req[strcspn(req, "\n")] = '\0';


        req_len = send(sock_FD, req, strlen(req) + 1, 0);
        if (req_len == -1)
            handle_break("Unable to send the code line");

        // the PRINT is handled separately because there can be more chunks receviced, being the possible len of a program grather than RES_LEN
        if (strcmp(req, PRINT) == 0)
        {
            printf(MAGENTA);
            while (1)
            {
                res_len = recv(sock_FD, res, RES_LEN, 0);
                if (res_len == -1)
                    handle_break("Unable to recv res"); 
                printf("%s", res);
                if (res_len < RES_LEN)
                    break;
            }
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
