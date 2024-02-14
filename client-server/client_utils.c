#include "client_utils.h"

void startCodingSession(int sock_FD, char req[], char res[])
{
    ssize_t req_len = 0, res_len = 0;
    printf("psychic-disco 1.0.0\n" 
    "Type \"help\" for more information\n");

    while (1)
    {
        printf(YELLOW BOLD ">>> ");

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
            printf(MAGENTA);
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
            printf(MAGENTA "Reached max program size: %d\n", PROGRAM_SIZE);
        else if (strcmp(res, EXEC_ERR) == 0)
            printf(MAGENTA "Program result: ERROR\n");
        else if (strcmp(res, EXEC_OK) == 0)
            printf(MAGENTA "Program result: RESULT\n");
        else if (strcmp(res, CLEAR_ERR) == 0)
            printf("Server unable to clear program\n");
        else if (strcmp(res, END_OK) == 0)
            break;
    }

    if (strcmp(res, END_OK) == 0)
        printf(MAGENTA "ok");

    printf(RESET "\n[TERMINAL_CLOSE]\n\n");
}

void showStartMsg(void)
{
    printf(CYAN "Connected on port %d.\n" RESET, PORT);
    printf("Type HELP in the req to know the commands\n\n");
}

