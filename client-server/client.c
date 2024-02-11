#include "client_utils.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

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
