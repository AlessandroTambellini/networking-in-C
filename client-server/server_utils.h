#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "common.h"

void 
openCodingSession(int sock_FD, char req[], char res[]);
void 
getHelpTxt(char res[]);
void 
execProgram(char *res);
void 
clearProgram(char program[], size_t *program_size, char *res);
void 
addLine(char program[], char req[], size_t req_len,  size_t *program_size, char *res);
void 
sendRes(int sock_FD, char res[], size_t res_len);
void
sendProgramChunks(int sock_FD, char *res, char *program);


#endif // SERVER_UTILS_H
