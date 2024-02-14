#ifndef COMMON_H
#define COMMON_H

#define  _GNU_SOURCE
#include <stdio.h> // ncurses.h includes stdio.h
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
// #include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000
#define REQ_LEN 256
#define RES_LEN 256

#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"
#define YELLOW "\e[0;33m"
#define MAGENTA "\e[0;35m"
#define RED "\e[0;31m"
#define BOLD "\e[1m"
#define RESET "\e[m"

// COMMANDS
// greet, just to test the server is able to respond
#define GREET "GREET" 
#define REQ_INVALID "REQ_INVALID"
// Open Coding Session
#define OCS "OCS"
#define CODE "CODE"
#define READ_ERR "READ_ERR"
#define ADD_OK "ADD_OK"
#define ADD_ERR "ADD_ERR"
#define PRINT "PRINT"
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
// list all the possible commands to the user
#define HELP "HELP"

#define PROGRAM_SIZE 2048
#define HELP_PATH "./help.txt"

#define handle_exit(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); exit(EXIT_FAILURE); } while (0)

#define handle_break(msg) \
    do { fprintf(stderr, RED msg RESET); perror(" "); break; } while (0)

#endif // COMMON_H
