#ifndef COMMON_H
#define COMMON_H

#define  _GNU_SOURCE
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
#define YELLOW "\e[0;33m"
#define RED "\e[0;31m"
#define BOLD "\e[1m"
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

// read: strlen
// recv: sizeof

#define PROGRAM_SIZE 5000
#define HELP_PATH "./help.txt"

#define handle_exit(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); exit(EXIT_FAILURE); } while (0)

#define handle_break(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); break; } while (0)

#endif // COMMON_H
