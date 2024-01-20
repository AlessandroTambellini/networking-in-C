#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000
#define REQ_LEN 500
#define RES_LEN 500

#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"
#define RESET "\e[m"

// COMMANDS
// just for testing purpose
#define GREET "GREET"
// Opening Coding Session
#define OCS "OCS"
#define CODE "CODE"
// end of code
#define END "END"
// close connection
#define CLOSE "CLOSE"
#define CLOSED "CLOSED"
// list all the possible commands to the user
#define HELP "HELP"

#define PROGRAM_SIZE 5000

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif // COMMON_H
