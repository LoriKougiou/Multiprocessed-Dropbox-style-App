#ifndef DIRMANAGEMENT_H
#define DIRMANAGEMENT_H

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

//domi listas pou krataei to onoma enos directory
typedef struct list_node{
	char* directory;
	struct list_node* next;
}list_node;


//domi listas pou krataei ta paths apo directories pou ksekinane apo kathe directory tou input dir
typedef struct firstLevel_node{
	list_node* start;
	struct firstLevel_node* next;
}firstLevel_node;


void dirDepthWalk(char *curr_dir, int buff_size, list_node** dirList);
firstLevel_node* traverseDirs(char *curr_dir, int buff_size);

void freeDirList(list_node* dirList);
void freeFLDirList(firstLevel_node* list);

#endif