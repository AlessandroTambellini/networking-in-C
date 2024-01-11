#include "common.h"

void manageExit(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}
