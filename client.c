#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
 
#define MAXRCVLEN 500
#define PORTNUM 2300
 
int main(int argc, char *argv[])
{
   char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
   struct sockaddr_in addr; 
 
   int mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
   memset(&addr, 0, sizeof(addr));                /* zero the struct */
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set IP number - localhost, 127.0.0.1 */ 
   addr.sin_port = htons(PORTNUM);                /* set port number */
 
   // use the mysocket to create a connection to the machine specified in addr
   if (connect(mysocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
      printf("Error: unable to create a connection.\n");
      exit(EXIT_FAILURE);
   }
  
   /* from the addr machine you receive a bunch of data to store in buffer
   It is important to note that the data received will not automatically be 
   null terminated when stored in the buffer, so we need to do it ourselves with 
   buffer[len] = '\0' */
   int len = recv(mysocket, buffer, MAXRCVLEN, 0);
   buffer[len] = '\0';

   printf("Received %s %d bytes.\n", buffer, len);
 
   close(mysocket);
   return EXIT_SUCCESS;
}
