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
#define MAGENTA "\e[0;35m"
#define RED "\e[0;31m"
#define BOLD "\e[1m"
#define RESET "\e[m"

// COMMANDS
// just for testing purpose
#define GREET "GREET" // req
// Open Coding Session
#define CODE "CODE" // req
#define OCS "OCS"  // res
#define READ_ERR "READ_ERR"
#define READ_OK "READ_OK"
// stop writing code
#define END "END" // req
#define ENDED "ENDED" // res
// close connection
#define CLOSE "CLOSE" // req
#define CLOSED "CLOSED" // res
// list all the possible commands to the user
#define HELP "HELP"

#define PROGRAM_SIZE 5000
#define HELP_PATH "./help.txt"

#define handle_exit(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); exit(EXIT_FAILURE); } while (0)

#define handle_break(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); break; } while (0)

#endif // COMMON_H
