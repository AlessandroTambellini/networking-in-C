#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define PORT 3000

#define GREEN "\e[0;32m"
#define CYAN "\e[0;36m"
#define RESET "\e[m"

void manageExit(char *str);

#endif // COMMON_H
