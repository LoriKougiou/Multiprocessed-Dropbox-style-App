#include "dirManagement.h"

//kanei free mia list apo dirs 
void freeDirList(list_node* dirList){
	list_node* temp=NULL, *curr=NULL;

	curr=dirList;

	while(curr!=NULL){
		free(curr->directory);
		temp=curr->next;
		free(curr);
		curr=temp;
	}
}

//kanei free to struct pou krataei ola list apo dirs me starting point ta dirs tou prwtou level tou input
void freeFLDirList(firstLevel_node* list){

	firstLevel_node* curr=NULL, *temp=NULL;

	curr=list;

	while(curr!=NULL){
		freeDirList(curr->start);
		temp=curr->next;
		free(curr);
		curr=temp;
	}
}

//perpata kata vathos apo ena dir tou input kai apothikevei ti lista
void dirDepthWalk(char *curr_dir, int buff_size, list_node** dirList){
    DIR *dir;
    struct dirent *ent;
    char path[1024];


    if (!(dir = opendir(curr_dir))) return;

    while ((ent=readdir(dir))!= NULL){

    	if(ent->d_type==DT_DIR){
    		if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0) continue;

    		if((*dirList)==NULL){
	    	(*dirList)=malloc(sizeof(list_node));
	    	if((*dirList)==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	(*dirList)->next=NULL;
	    	(*dirList)->directory=NULL;
    }

	        snprintf(path, sizeof(path), "%s/%s", curr_dir, ent->d_name);
	        (*dirList)->directory=malloc((strlen(path)+1)*sizeof(char));
	        if((*dirList)->directory==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	strcpy((*dirList)->directory,path);

	        dirDepthWalk(path, buff_size, &((*dirList)->next));
    	}               
    }

    closedir(dir);
}

//ftiaxnei to struct me ola ta dir lists pou exoun enarktirio dir ena apo ta dirs pou vriskontai sto input dir
firstLevel_node* traverseDirs(char *curr_dir, int buff_size){
    DIR *dir;
    struct dirent *ent;
    char path[1024];
    list_node* curr;
    firstLevel_node* list=NULL, *currFlevel=NULL, *prev=NULL;

    list=malloc(sizeof(firstLevel_node));
    if(list==NULL){
    	fprintf(stderr, "Malloc failed.\n");
    	exit(1);
    }
    list->start=NULL;
    list->next=NULL;
    currFlevel=list;

    if (!(dir = opendir(curr_dir))) return NULL;

    while ((ent=readdir(dir))!= NULL){

    	if(ent->d_type==DT_DIR){
    		if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0) continue;


    		if(currFlevel==NULL){
    			currFlevel=malloc(sizeof(firstLevel_node));
			    if(currFlevel==NULL){
			    	fprintf(stderr, "Malloc failed.\n");
			    	exit(1);
			    }
			    currFlevel->start=NULL;
			    currFlevel->next=NULL;
    		}
    		
	    	currFlevel->start=malloc(sizeof(list_node));
	    	if(currFlevel->start==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	currFlevel->start->next=NULL;
	    	currFlevel->start->directory=NULL;

	    	curr=currFlevel->start;

	    	//apothikevw to input stin arxi kathe path
	    	curr->directory=malloc((strlen(curr_dir)+1)*sizeof(char));
	        if(curr->directory==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	strcpy(curr->directory,curr_dir);

	    	curr=curr->next;
	    	curr=malloc(sizeof(list_node));
	    	if(curr==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	curr->next=NULL;
	    	curr->directory=NULL;

	    	if(prev!=NULL) prev->next=currFlevel;

	    	//kai twra apo ta nested directories kai pera
	        snprintf(path, sizeof(path), "%s/%s", curr_dir, ent->d_name);
	        curr->directory=malloc((strlen(path)+1)*sizeof(char));
	        if(curr->directory==NULL){
	    		fprintf(stderr, "Malloc failed.\n");
	    		exit(1);
	    	}
	    	strcpy(curr->directory,path);
	    	currFlevel->start->next=curr;
	        dirDepthWalk(curr->directory, buff_size, &(curr->next));
	        prev=currFlevel;
	        currFlevel=currFlevel->next;
    	}               
    }
    closedir(dir);
    return list;
}
