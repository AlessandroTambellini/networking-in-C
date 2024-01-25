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
// just to test the server is able to respond
#define GREET "GREET" // req
// Open Coding Session
#define CODE "CODE" // req
#define OCS "OCS"  // res
#define READ_ERR "READ_ERR"
#define ADD_OK "ADD_OK"
#define ADD_ERR "ADD_ERR"
// clear code written so far
#define CLEAR "CLEAR"
#define CLEAR_OK "CLEAR_OK"
#define CLEAR_ERR "CLEAR_ERR"
// exec program
#define EXEC "EXEC"
#define EXEC_OK "EXEC_OK"
#define EXEC_ERR "EXEC_ERR"
// stop writing code
#define END "END" // req
#define END_OK "END_OK" // res
// close connection
#define CLOSE "CLOSE" // req
#define CLOSE_OK "CLOSE_OK" // res
// list all the possible commands to the user
#define HELP "HELP"

#define PROGRAM_SIZE 5000
#define HELP_PATH "./help.txt"

#define handle_exit(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); exit(EXIT_FAILURE); } while (0)

#define handle_break(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); break; } while (0)

#endif // COMMON_H
