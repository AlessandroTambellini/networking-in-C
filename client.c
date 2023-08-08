#include "common.h"

#define MAXRCVLEN 500

int main(int argc, char *argv[])
{
    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    struct sockaddr_in addr;

    int mysocket = socket(AF_INET, SOCK_STREAM, 0);

    // address setup
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(PORTNUM);

    // use the mysocket to create a connection to the machine specified in addr
    if (connect(mysocket, (sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
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
