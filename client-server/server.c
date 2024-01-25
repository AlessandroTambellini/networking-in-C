#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "common.h"

#define HOSTNAME_SIZE 100
#define LISTEN_BACKLOG 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void openCodingSession(int sock_FD, char req[], char res[]);
void getHelpTxt(char res[]);
char *execProgram(char program[]);
char *clearProgram(char program[]);
char *addLine(char program[],char req[], size_t program_len, size_t req_len);

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

    int server_socket_FD = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_FD == -1) 
        handle_exit("Unable to create socket");

    if (bind(server_socket_FD, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1) 
        handle_exit("Unable to bind");

    // start listening, and just 1 client can connect
    if (listen(server_socket_FD, LISTEN_BACKLOG) == -1) 
        handle_exit("Unable to listen for new connections");

    printf(GREEN "Listening on port %d for incoming requests\n" RESET, PORT);

    char req[REQ_LEN];
    char res[RES_LEN];

    int client_socket_FD = accept(server_socket_FD, (sockaddr *)&client_addr, &client_addr_size);
    
    if (client_socket_FD == -1) 
        handle_exit("Unable to open new socket");
    printf("Client connected from IP %s from port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1)
    {   
        int isReqRead = read(client_socket_FD, req, REQ_LEN);
        if (isReqRead == -1)
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
            strcpy(res, "not a valid req");

        // 2) send the res
        int isResSent = send(client_socket_FD, res, strlen(res) + 1, 0);
        if (isResSent == -1)
            handle_break("Unable to send res to client");
        printf(GREEN "res sent: ok\n" RESET);
    }

    if (strcmp(req, CLOSE) == 0)
        strcpy(res, CLOSE_OK);
    int isResSent = send(client_socket_FD, res, strlen(res) + 1, 0);
    if (isResSent == -1)
        handle_break("Unable to send res to client");
    printf(GREEN "res sent: ok\n" RESET);

    close(client_socket_FD);
    close(server_socket_FD);
}

void openCodingSession(int sock_FD, char req[], char res[])
{
    strcpy(res, OCS);
    int isResSent = send(sock_FD, res, strlen(res) + 1, 0);
    if (isResSent == -1)
        handle_exit("Unable to send res to client");
    printf(GREEN "res sent: ok\n" RESET);

    char program[PROGRAM_SIZE] = {0};

    while (1)
    {
        int isReqRead = read(sock_FD, req,  REQ_LEN);
        if (isReqRead == -1)
            strcpy(res, READ_ERR);
        
        // 1) read req, act consequently and write the res
        if (strcmp(req, EXEC) == 0)
            strcpy(res, execProgram(program));
        else if (strcmp(req, CLEAR) == 0)
            strcpy(res, clearProgram(program));
        else if (strcmp(req, END) == 0)
            break;
        else
            strcpy(res, addLine(program, req, strlen(program), strlen(req)));

        // 2) send the res
        int isResSent = send(sock_FD, res, strlen(res) + 1, 0);
        if (isResSent == -1)
            handle_break("Unable to send res to client");
        printf(MAGENTA "res sent: ok\n" RESET);
    }

    if (strcmp(req, END) == 0)
        strcpy(res, END_OK);
    isResSent = send(sock_FD, res, strlen(res) + 1, 0);
    if (isResSent == -1)
        handle_break("Unable to send res to client");
    printf(GREEN "res sent: ok\n" RESET);
}

void getHelpTxt(char res[])
{
    FILE *help_file = fopen(HELP_PATH, "r");
    if (help_file == NULL)
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

char *execProgram(char program[])
{
    // fake res
    return EXEC_OK;
    // TODO: build the compiler
}

char *clearProgram(char program[])
{
    memset(program, 0, PROGRAM_SIZE);
    if (strlen(program) == 0)
        return CLEAR_OK;
    else
        return CLEAR_ERR; 
}

char *addLine(char program[], char req[], size_t program_len, size_t req_len)
{
    if ((program_len + req_len + 1) > PROGRAM_SIZE)
        return ADD_ERR;

    strcat(program, req);
    return ADD_OK;
}
