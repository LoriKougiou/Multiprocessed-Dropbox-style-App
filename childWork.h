#ifndef CHILDWORK_H
#define CHILDWORK_H

#include "hash.h"
#include "readwrite.h"
#include "sighandle.h"

void senderWork(char* sender_fifo, char* mirror, char* input, char* found_str, char* logfile, int buff_size, FILE* fp);
void receiverWork(char* receiver_fifo, char* mirror, char* input, char* found_str, char* logfile, int buff_size, FILE* fp);

#endif