#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

#define HOSTNAME_SIZE 100
#define LISTEN_BACKLOG 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void 
openCodingSession(int sock_FD, char req[], char res[]);
void 
getHelpTxt(char res[]);
void 
execProgram(char *res);
void 
clearProgram(char program[], size_t *program_size, char *res);
void 
addLine(char program[], char req[], size_t req_len,  size_t *program_size, char *res);
void 
sendRes(int sock_FD, char res[], size_t res_len);
void
sendProgramChunks(int sock_FD, char *res, char *program);

int 
main(void)
{
    sockaddr_in server_addr = {0}, client_addr = {0};

    server_addr = (sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    socklen_t client_addr_size = sizeof(sockaddr_in);

    int opt = 1;
    int server_socket_FD = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_FD == -1) 
        handle_exit("Unable to create socket");

    // Set socket option to allow address reuse --> avoid "Unable to bind : Address already in use" error
    if (setsockopt(server_socket_FD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        handle_exit("Unable to reuse address");
    }

    if (bind(server_socket_FD, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1) 
        handle_exit("Unable to bind");

    if (listen(server_socket_FD, LISTEN_BACKLOG) == -1) 
        handle_exit("Unable to listen for new connections");

    printf(GREEN "Listening on port %d for incoming requests\n" RESET, PORT);

    int client_socket_FD = accept(server_socket_FD, (sockaddr *)&client_addr, &client_addr_size);
    
    if (client_socket_FD == -1) 
        handle_exit("Unable to open new socket");
    printf("Client connected from IP %s from port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char req[REQ_LEN], res[RES_LEN];
    ssize_t req_len = 0;
    while (1)
    {   
        req_len = read(client_socket_FD, req, REQ_LEN);
        if (req_len == -1)
            handle_break("Unable to read client req");

        printf(CYAN "req: %s\n" RESET, req);

        // 1) write the res
        if (strcmp(req, GREET) == 0)
            strcpy(res, "Hello!");
        else if (strcmp(req, CODE) == 0)
            openCodingSession(client_socket_FD, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
        else if (strcmp(req, HELP) == 0)
            getHelpTxt(res);
        else
            strcpy(res, REQ_INVALID);

        // 2) send the res
        sendRes(client_socket_FD, res, strlen(res) + 1);
    }

    // close(client_socket_FD);
    close(server_socket_FD);
    printf("Connection closed.\n");
}

void openCodingSession(int sock_FD, char req[], char res[])
{
    strcpy(res, OCS);
    sendRes(sock_FD, res, strlen(res) + 1);

    ssize_t req_len = 0;
    // I need to use a variable instead of strlen(program) because otherwise I cannot calculate the program length inside a function
    size_t program_size = 0;
    char program[PROGRAM_SIZE] = {0};

    while (1)
    {
        req_len = read(sock_FD, req,  REQ_LEN);
        if (req_len == -1)
            strcpy(res, READ_ERR);
        
        // 1) read req, act consequently and write the res
        if (strcmp(req, EXEC) == 0)
            execProgram(res);
        else if (strcmp(req, PRINT) == 0)
        {
            //  TOREVIEW: send the program in chunks of 255 chars

            if (program_size == 0)
            {
                strcpy(res, program);
                sendRes(sock_FD, res, strlen(res) + 1);
            }

            size_t start_idx = 0, chunk_len = 0;
            while (start_idx < program_size)
            {
                chunk_len = program_size - start_idx;
                if (chunk_len > (REQ_LEN - 1))
                    strncpy(res, &program[start_idx], REQ_LEN - 1);
                else
                    strcpy(res, &program[start_idx]);
                
                sendRes(sock_FD, res, strlen(res) + 1);
                start_idx += (REQ_LEN - 1);
            }
        }
        else if (strcmp(req, CLEAR) == 0)
            clearProgram(program, &program_size, res);
        else if (strcmp(req, END) == 0)
            break;
        else
            addLine(program, req, strlen(req), &program_size, res);

        // 2) send the res
        // the send for PRINT was already handled
        if (strcmp(req, PRINT) != 0)
            sendRes(sock_FD, res, strlen(res) + 1);     
    }

    if (strcmp(req, END) == 0)
        strcpy(res, END_OK);
    sendRes(sock_FD, res, strlen(res) + 1);
}

void getHelpTxt(char res[])
{
    FILE *help_file = fopen(HELP_PATH, "r");
    if (NULL == help_file)
    {
        strcpy(res, "Sorry, unable to display the help guide");
        return;
    }

    // get file size
    fseek(help_file, 0, SEEK_END);
    size_t help_len = ftell(help_file);
    fseek(help_file, 0, SEEK_SET);

    res[0] = '\n';
    fread(res + 1, sizeof(char), help_len, help_file);
    res[help_len] = '\0';

    fclose(help_file);
}

void execProgram(char *res)
{
    // fake res
    strcpy(res, EXEC_OK);
    // TODO: build the compiler
}

void clearProgram(char program[], size_t *program_size, char *res)
{
    memset(program, 0, PROGRAM_SIZE);
    *program_size = 0;
    if (strlen(program) == 0)
        strcpy(res, CLEAR_OK);
    else
        strcpy(res, CLEAR_ERR);
}

void addLine(char program[], char req[], size_t req_len, size_t *program_size, char *res)
{
    if ((*program_size + req_len + 1) > PROGRAM_SIZE)
        strcpy(res, ADD_ERR);

    strcat(program, req);
    *program_size += req_len;
    strcpy(res, ADD_OK);
}

void sendRes(int sock_FD, char res[], size_t res_len)
{
    if (send(sock_FD, res, res_len, 0) == -1)
        handle_exit("Unable to send res to client");
    printf(GREEN "res sent: ok\n" RESET);
}

void sendProgramChunks(int sock_FD, char *res, char *program)
{

}
