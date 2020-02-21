#include "sighandle.h"

//synarthsh pou pianei ta signals kai thetei thn volatile metavliti signaled katallila gia na kserei pws na to diaxeiristei o pateras
void myHandler(int signum, siginfo_t *siginfo, void *context){
    if (signum == SIGUSR1){
		signaled=1;
		sigSender=siginfo->si_pid;
    }    
    else if(signum == SIGSEGV){
    	write(1,"Received SIGSEGV\n",strlen("Received SIGSEGV\n")*sizeof(char));
		exit(1);
    }
    else if(signum == SIGINT){
    	write(1,"Received SIGINT\n",strlen("Received SIGINT\n")*sizeof(char));
		signaled=3;
    }
    else if(signum == SIGQUIT){
    	write(1,"Received SIGQUIT\n",strlen("Received SIGQUIT\n")*sizeof(char));
		signaled=4;
    }
    else if(signum== SIGUSR2){
        signaled=5;
        sigSender=siginfo->si_pid;
    }
    else if(signum == SIGPIPE){

        //kanw ignore auto to signal giati exei dimiourgithei apo to sigusr allou process
    }
    else if(signum == SIGTERM){

        //kanw ignore auto to signal giati ginetai triger otan to allo paidi stelnei sigusr kai o pateras skotwnei auto to paidi
    }
}


//apodesmevei ton xwro pou pianei lista poy krataei ta paidia kai ta retries
void free_SigList(sigList_node* List){
    sigList_node* temp=NULL, *curr=NULL;

    curr=List;

    while(curr!=NULL){
        temp=curr->next;
        free(curr);
        curr=NULL;
        curr=temp;
    }
}

