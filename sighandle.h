#ifndef SIGHANDLE_H
#define SIGHANDLE_H

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

//metavlhtes pou thetei o signal handler analogws gia na voithisei ton patera na xeiristei to kathe signal
volatile sig_atomic_t signaled;
volatile sig_atomic_t sigSender;


//domh lista pou krataei gia kathe neo client pou emfanizetai sto common kai ta id twn paidiwn pou dimiourgei o client mou gia th metafora, 
//to retry count kai to id tou neou client
typedef struct sigList_node{
	pid_t sender;
	pid_t receiver;
	int retry_count;
	int id;
	struct sigList_node* next;
}sigList_node;

void myHandler(int signum, siginfo_t *siginfo, void *context);
void free_SigList(sigList_node* List);

#endif