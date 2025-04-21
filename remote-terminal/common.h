#ifndef COMMON_H
#define COMMON_H

#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
// #include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000

#define MAX_REQ_LEN 1024
#define MAX_RES_LEN 1024

#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"
#define YELLOW "\e[0;33m"
#define MAGENTA "\e[0;35m"
#define RED "\e[0;31m"
#define BOLD "\e[1m"
#define RESET "\e[m"

// COMMANDS
// user commands are lowercase
// server responses are UPPERCASE
// greet just to test connection
#define GREET "greet" 
#define REQ_INVALID "REQ_INVALID"
// Open Coding Session
#define OCS "OCS"
#define CODE "code"
#define READ_ERR "READ_ERR"
#define ADD_OK "ADD_OK"
#define ADD_ERR "ADD_ERR"
#define PRINT "print"
// clear code written so far
#define CLEAR "clear"
#define CLEAR_OK "CLEAR_OK"
#define CLEAR_ERR "CLEAR_ERR"
// exec program
#define EXEC "exec"
#define EXEC_OK "EXEC_OK"
#define EXEC_ERR "EXEC_ERR"
// stop writing code
#define END "end" // req
#define END_OK "END_OK" // res
// close connection
#define CLOSE "close" // req
// list all the possible commands to the user
#define HELP "help"

#define PROGRAM_SIZE 2048
#define HELP_PATH "./help.txt"

#define handle_exit(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); exit(EXIT_FAILURE); } while (0)

#define handle_break(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); break; } while (0)

#endif // COMMON_H
