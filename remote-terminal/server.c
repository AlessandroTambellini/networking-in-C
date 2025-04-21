#include <arpa/inet.h>

#include "common.h"

#define HOSTNAME_SIZE 100
#define LISTEN_BACKLOG 1

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

size_t program_size = 0;
char program[PROGRAM_SIZE] = {0};

void open_coding_session(int sock_FD, char req[], char res[]);

void get_help_txt(char res[]);
void exec_program(char *res);
void clear_program(char program[], size_t *program_size, char *res);
void add_line(char program[], char req[], size_t req_len, size_t *program_size, char *res);
void send_res(int sock_FD, char res[], size_t res_len);

int 
main(void)
{
    sockaddr_in server = {0}, client = {0};

    server = (sockaddr_in) {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    socklen_t addr_size = sizeof(sockaddr_in);

    int opt = 1;
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) 
        handle_exit("Unable to create socket");

    // Set socket option to allow address reuse --> avoid "Unable to bind : Address already in use" error
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        handle_exit("Unable to reuse address");

    if (bind(server_sock, (sockaddr *)&server, sizeof(sockaddr)) == -1) 
        handle_exit("Unable to bind");

    if (listen(server_sock, LISTEN_BACKLOG) == -1) 
        handle_exit("Unable to listen for new connections");

    printf(GREEN "Listening on port %d for incoming requests.\n" RESET, PORT);

    int client_sock = accept(server_sock, (sockaddr *)&client, &addr_size);
    
    if (client_sock == -1) {
        handle_exit("Unable to open new socket");
    } else {
        printf("Client connected from IP %s on port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    }

    while (1)
    {   
        char req[MAX_REQ_LEN] = { 0 }; 
        char res[MAX_RES_LEN] = { 0 };

        if (read(client_sock, req, MAX_REQ_LEN) == -1) {
            strcpy(res, "The server is not able to read the request.");
            send_res(client_sock, res, strlen(res) + 1);
            handle_break("Unable to read client req.");
        }

        printf(CYAN "req: %s\n" RESET, req);

        if (strcmp(req, GREET) == 0)
            strcpy(res, "Hello!");
        else if (strcmp(req, CODE) == 0)
            open_coding_session(client_sock, req, res);
        else if (strcmp(req, CLOSE) == 0)
            break;
        else if (strcmp(req, HELP) == 0)
            get_help_txt(res);
        else
            strcpy(res, REQ_INVALID);

        send_res(client_sock, res, strlen(res) + 1);
    }

    // close(client_sock);
    close(server_sock);
    printf("Connection closed.\n");
}

void open_coding_session(int sock_FD, char req[], char res[])
{
    strcpy(res, OCS);
    send_res(sock_FD, res, strlen(res) + 1);

    while (1)
    {
        if (read(sock_FD, req,  MAX_REQ_LEN) == -1)
            strcpy(res, READ_ERR);
        
        // 1) read req, act consequently and write the res
        if (strcmp(req, EXEC) == 0)
            exec_program(res);
        else if (strcmp(req, PRINT) == 0)
        {
            if (program_size == 0)
            {
                strcpy(res, "");
                send_res(sock_FD, res, strlen(res) + 1);
                continue;
            }

            size_t start_idx = 0, chunk_len = 0;
            while (start_idx < program_size)
            {
                chunk_len = program_size - start_idx;
                if (chunk_len > (MAX_RES_LEN - 1))
                    strncpy(res, &program[start_idx], MAX_RES_LEN - 1);
                else
                    strcpy(res, &program[start_idx]);
                
                send_res(sock_FD, res, strlen(res) + 1);
                start_idx += (MAX_RES_LEN - 1);
            }
        }
        else if (strcmp(req, CLEAR) == 0)
            clear_program(program, &program_size, res);
        else if (strcmp(req, END) == 0)
            break;
        else
            add_line(program, req, strlen(req), &program_size, res);

        // 2) send the res (the res for PRINT was already sent)
        if (strcmp(req, PRINT) != 0)
            send_res(sock_FD, res, strlen(res) + 1);     
    }

    if (strcmp(req, END) == 0) {
        strcpy(res, END_OK);
        send_res(sock_FD, res, strlen(res) + 1);
    }
}

void get_help_txt(char res[])
{
    FILE *help_file = fopen(HELP_PATH, "r");
    if (!help_file) {
        strcpy(res, "Sorry, unable to display the help guide.");
        return;
    }

    // get file size
    fseek(help_file, 0, SEEK_END);
    size_t file_size = ftell(help_file);
    fseek(help_file, 0, SEEK_SET);

    if (file_size < MAX_REQ_LEN) {
        fread(res, sizeof(char), file_size, help_file);
    } else {
        printf("[WARN]: content size of help.txt (%zu) exceeds MAX_REQ_LEN (%u).\n", file_size, MAX_REQ_LEN);
        // For now 1KB is enough. It's a warning for myself
    }

    fclose(help_file);
}

void exec_program(char *res)
{
    // fake res
    strcpy(res, EXEC_OK);
    // TODO: build the interpreter
}

void clear_program(char program[], size_t *program_size, char *res)
{
    memset(program, 0, PROGRAM_SIZE);
    *program_size = 0;
    if (strlen(program) == 0)
        strcpy(res, CLEAR_OK);
    else
        strcpy(res, CLEAR_ERR);
}

void add_line(char program[], char line[], size_t line_size, size_t *program_size, char *res)
{
    if ((*program_size + line_size + 1) > PROGRAM_SIZE)
        strcpy(res, ADD_ERR);

    strcat(program, line);
    strcat(program, "\n");
    *program_size += (line_size + 1);
    strcpy(res, ADD_OK);
}

void send_res(int sock_FD, char res[], size_t res_len)
{
    if (send(sock_FD, res, res_len, 0) == -1)
        handle_exit("Unable to send res to client.");
    // printf(GREEN "res sent: ok\n" RESET);
}
