#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000

#define HANDLE_ERROR(msg) \
    perror(msg); \
    exit(EXIT_FAILURE)

#endif
