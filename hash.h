#ifndef HASH_H
#define HASH_H

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

typedef struct b_entry{
	int id;
	int status;
}b_entry;

typedef struct bucket{
	int numOfEntries;
	b_entry* arr;
	struct bucket* next;
}bucket;

int hash_func(int key);
void insert_key(bucket*** idHash, int key, int maxBuckEnts, int status);
void free_hashTable(bucket** hashTable);

#endif