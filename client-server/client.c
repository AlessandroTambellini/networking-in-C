#include <netinet/in.h>
#include <string.h>
#include "common.h"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int main(void) 
{
    int client_socket_FD = socket(AF_INET, SOCK_STREAM, 0); 
  
    sockaddr_in server_addr = {
      .sin_family = AF_INET, // IPv4
      .sin_port = htons(PORT),
      .sin_addr.s_addr = INADDR_ANY, // any IP address from local machine
    };  

    int isConnected = connect(client_socket_FD, (sockaddr*)&server_addr, sizeof(server_addr)); 
  
    if (isConnected == -1) 
      manageExit("Unable to connect socket");

    printf("%sConnected to server on port %d successfully\n%s", CYAN, PORT, RESET);

    char res[RES_LEN]; 
    char req[REQ_LEN];
  
    while (1)
    {
      printf("req: ");
      scanf("%s", req);

      int isMsgSent = send(client_socket_FD, req, strlen(req) + 1, 0); // + 1: to also send \0
      if (isMsgSent == -1)
      {
        perror("Unable to send msg to server");
        // close(client_socket_FD);
      }
      else
      {
        recv(client_socket_FD, res, sizeof(res), 0); 
        printf("%sres: %s\n%s", CYAN, res, RESET); 
      }
    }

    close(client_socket_FD);    
}
