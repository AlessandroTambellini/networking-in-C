#include <stdio.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>

#define PORT "3000"
#define MSG_SIZE 1000

int main(void)
{
    // establish connection to server
    int fd = create_inet_stream_socket("localhost", PORT, LIBSOCKET_IPv4, 0);
    FILE *socket = fdopen(fd, "r");
    
    //  greeting
    char res[MSG_SIZE];
    fgets(res, MSG_SIZE, socket);
    printf("%s", res);

    getchar();
    
    fclose(socket);
    close(fd);
}
