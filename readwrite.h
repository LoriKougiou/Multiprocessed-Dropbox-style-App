#ifndef READWRITE_H
#define READWRITE_H

#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/inotify.h>
#include <fcntl.h> 
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

#include "dirManagement.h"


int reader(char* fifofile, char* mirror, char* logfile, int buff_size);
int writer(int buff_size, char* fifofile, char* input, char* logfile);

char* parseFilename(int filename_len, char* buffer, char* mirror);

#endif