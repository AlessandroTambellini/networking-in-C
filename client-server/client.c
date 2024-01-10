#include <netinet/in.h>
#include "common.h"

#define MSG_SIZE 1000

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
  
int main(void) 
{ 
    HANDLE_ERROR("myerroooooor");
    
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  
    sockaddr_in server_addr = {
      .sin_family = AF_INET, // IPv4
      .sin_port = htons(PORT),
      .sin_addr.s_addr = INADDR_ANY, // any IP address from local machine
    }; 

    int connection  = connect(socketFD, (sockaddr*)&server_addr, sizeof(server_addr)); 
  
    if (connection == -1) 
        HANDLE_ERROR("Unable to connect socket");
  
    char res[MSG_SIZE]; 
    recv(socketFD, res, sizeof(res), 0); 
  
    printf("Server response: %s\n", res); 
}
