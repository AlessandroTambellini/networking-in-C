#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <stdio_ext.h>

#include "common.h"

void
showStartMsg(void);
void 
startCodingSession(int sock_FD, char req[], char res[]);

#endif // CLIENT_UTILS_H
