#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define MAX_HTML_SIZE 64*1024
#define PORT 3000

typedef struct sockaddr sockaddr;

#define MAX_HOSTNAME_SIZE 100

void get_HTML(char *HTML);

int main(void)
{
    struct sockaddr_in client;
    struct sockaddr_in server;

    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1) {
        switch (errno) {
        case EPROTONOSUPPORT:
            printf("Error: protocol not supported.\n");
            break;
        case EACCES:
            printf("Error: permission denied.\n");
            break;
        default:
            printf("Error: unknown error in socket function.\n");
            break;
        }
        exit(EXIT_FAILURE);
    }

    if (bind(listen_socket, (sockaddr *)&server, sizeof(sockaddr)) == -1) {
        printf("Error: unable to bind.\n");
        exit(EXIT_FAILURE);
    }

    /* listen(socket, backlog). backlog = max num of connections
    the socket will queue up, each waiting to be accepted. */
    if (listen(listen_socket, 1) == -1) {
        printf("Error: unable to listen for new connections.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Listening on port %d.\n", PORT);
    }

    char HTML[MAX_HTML_SIZE];
    
    while (1) 
    {
        int conn_socket = accept(listen_socket, (sockaddr *)&client, &socksize);
        if (conn_socket == -1) {
            printf("Error: unable to open new socket.\n");
            exit(EXIT_FAILURE);
        }

        printf("Incoming connection from %s\n", inet_ntoa(client.sin_addr));
        
        get_HTML(HTML);
        send(conn_socket, HTML, strlen(HTML), 0);
        close(conn_socket);
    }

    close(listen_socket);

    return 0;
}

void get_HTML(char *HTML)
{
    char *HTML_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html;\r\n\r\n";
    strcpy(HTML, HTML_header);

    FILE *HTML_p = fopen("index.html", "r");

    while (fgets(HTML + strlen(HTML), MAX_HTML_SIZE - strlen(HTML), HTML_p));

    fclose(HTML_p);
}
