#include "checkDirs.h"

//psaxnei to hash kai elegxei an to o client exei idi sigronistei me to dothen id. ean de to vrei epistrefei -1
int syncedWithId(bucket*** idHash, int id, bucket** toWrite){
	bucket* curr=NULL;
	int i, hash_val;

	if((*idHash)==NULL) return -1;

	hash_val=hash_func(id);
	curr=(*idHash)[hash_val];
	*toWrite=curr;

	while(curr!=NULL){
		for(i=0;i<curr->numOfEntries;i++){
			if(curr->arr[i].id==id) return i;
		}
		curr=curr->next;
		if(curr!=NULL) *toWrite=curr;
	}
	return -1;
}


//checkarei ean iparxei arxeio me auto to onoma idi sto directory pou exei dothei
int fileExists(char *path){
    if (access(path, F_OK) == -1) return 0;
    return 1;
}

//synarthsh pou kanei tous arxikous elegxous gia ta dosmena dirs
void initDirCheck(char* input, char* common, char* mirror, char* logfile){
	FILE* fp;
	DIR* dir;

	//anoigw to logfile gia na grapsw ta minimata
	fp=fopen(logfile, "w+");

	//Arxika elegxw an iparxei to input directory.
	dir=opendir(input);

	if(dir) {
		fprintf(fp, "The input directory %s exists.\n", input);
		closedir(dir); //ean iparxei eimaste okay, opote apla to klinw
	}
	else if((dir==NULL)&&(ENOENT==errno)){
	    fprintf(fp, "The input directory %s does not exist. Exiting program.\n", input);
	    exit(1);
	}
	else {
		fprintf(fp, "The input directory %s exists but open failed.Exiting program.\n",input);
		exit(1);
	}

	//Twra elegxw an iparxei to mirror directory
	dir=opendir(mirror);

	if(dir){
		fprintf(fp, "The mirror directory %s already exists, probably being used by another client. Exiting program.\n", mirror);
	    exit(1);
	} 
	else if((dir==NULL)&&(ENOENT==errno)){ //to mirror directory den iparxei opote to dimiourgw
		if(mkdir(mirror, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1){
			fprintf(fp, "Creation of mirror directory %s failed. Exiting program.\n", mirror);
	    	exit(1);
		}
		else {
			fprintf(fp, "The mirror directory %s was successfully created.\n", mirror);
			closedir(dir);
		}
	}

	//Twra elegxw an iparxei to common directory
	dir=opendir(common);

	if(dir){
		fprintf(fp, "The common directory %s already exists.\n", common);
	    closedir(dir);
	} 
	else if((dir==NULL)&&(ENOENT==errno)){ //to common directory den iparxei opote to dimiourgw
		if(mkdir(common, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1){
			fprintf(fp, "Creation of common directory %s failed. Exiting program.\n", common);
	    	exit(1);
		}
		else {
			fprintf(fp, "The common directory %s was successfully created.\n", common);
			closedir(dir);
		}
	}

	fclose(fp);
}

//synarthsh pou dimiourgei to .id file mesa sthn common
void createIdFile(int client_id, char* common, char* logfile){
	char* path=NULL, id_str[10];
	FILE* fpcom, *fplog;

	if((common==NULL)||(logfile==NULL)){
		fprintf(stderr, "Given string is empty.Exiting program.\n");
		exit(1);
	} 

	sprintf(id_str, "%d", client_id); 
	strcat(id_str, ".id");

	path=malloc((strlen(id_str)+strlen(common)+4)*sizeof(char));
	if(path==NULL){
		fprintf(stderr, "Malloc failed.\n");
		exit(1);
	}
	strcpy(path,common);
	strcat(path,"/");
	strcat(path,id_str);

	fplog=fopen(logfile,"a");

	if(fileExists(path)){
		fprintf(fplog, "Client id %d is already in use. Exiting program.\n",client_id);
		fclose(fplog);
		exit(1);
	}
	else{
		fpcom=fopen(path,"w+");
		fprintf(fpcom, "%d", getpid());
		fclose(fpcom);
		fprintf(fplog, "NEW_CLIENT: %d\n",client_id);
	}

	fclose(fplog);
	free(path);
}

//elegxei to common directory gia neous client i gia apoxwriseis , kai asxoleitai kai me ta signals pou dexetai kata ti diarkeia
int checkCommon(char* common, char* logfile,char* input,char* mirror, bucket*** idHash, int maxBuckEnts, int id, int buff_size, int* prev_status, sigList_node** sigList, sigList_node** prevSig_node){
	FILE* fp;
	DIR* com_dir=NULL;
	int synced, k, j, i, status, found_id, stat;
	bucket* currBuck=NULL;
	struct dirent* ent;
	pid_t receiver, sender, wpid;
	sigList_node *currSig_node=NULL;
	char sender_fifo[25], receiver_fifo[25], fullpath[50], found_str[20], id_str[20];
	strcpy(fullpath, input);

	sprintf(id_str, "%d", id);
	status=(*prev_status)+1;
	fp=fopen(logfile, "a");

	
	//kanw set up ton handler gia ton patera
	struct sigaction act; 
	memset (&act, 0, sizeof(act));
	act.sa_sigaction = &myHandler;
	sigfillset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

	if(sigaction(SIGUSR1, &act, NULL)==-1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	if(sigaction(SIGINT, &act, NULL)==-1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	if(sigaction(SIGQUIT, &act, NULL)==-1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	if(sigaction(SIGSEGV, &act, NULL)==-1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	if(sigaction(SIGUSR2, &act, NULL)==-1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}


	

	if((com_dir=opendir(common))!= NULL){

		
		while ((ent=readdir(com_dir))!=NULL){
			if((ent->d_type!=DT_DIR)&&(ent->d_type!=DT_FIFO)){

				found_id=makeId(ent->d_name);
				sprintf(found_str, "%d", found_id);

				synced=syncedWithId(idHash, found_id, &currBuck);
				if(synced==-1){
					fprintf(fp, "NEW_CLIENT: %d\n",found_id);
					fprintf(stderr, "Client %d is new, starting synchronization.\n", found_id);
					insert_key(idHash, found_id , maxBuckEnts, status);
					fprintf(stderr, "The id of the initial process is %d\n",getpid());


					//ftiaxnw ta onomata twn fifo
					strcpy(sender_fifo,common);
					strcat(sender_fifo,"/");
					strcat(sender_fifo,id_str);
					strcat(sender_fifo,"_to_");
					strcat(sender_fifo,found_str);


					strcpy(receiver_fifo,common);
					strcat(receiver_fifo,"/");
					strcat(receiver_fifo,found_str);
					strcat(receiver_fifo,"_to_");
					strcat(receiver_fifo,id_str);					
					

					//desmevw mnimi gia tin sigList pou krataei ousiastika ola ta ta sigusrs pou lamvanei o parent process .tha ta xreiastw gia ton elegxo
					if((*sigList)==NULL){
						(*sigList)=malloc(sizeof(sigList_node));
						if((*sigList)==NULL){
							fprintf(stderr, "Malloc failed.\n");
							exit(1);
						}
						currSig_node=(*sigList);		
					} 
					else{
						if((*prevSig_node)==NULL){
							currSig_node=malloc(sizeof(sigList_node));
							if(currSig_node==NULL){
								fprintf(stderr, "Malloc failed.\n");
								exit(1);
							}
							(*prevSig_node)=(*sigList);

						}
						else{
							currSig_node=malloc(sizeof(sigList_node));
							if(currSig_node==NULL){
								fprintf(stderr, "Malloc failed.\n");
								exit(1);
							}
							(*prevSig_node)->next=currSig_node;
						}
						
					}

					currSig_node->id=found_id;
					currSig_node->next=NULL;

					//kanw fork ta sender, receiver paidia
					sender=fork();

					if (sender==0){


						//kanw setup ton handler gia ton sender
						if(sigaction(SIGPIPE, &act, NULL)==-1){
							perror("sigaction");
							exit(EXIT_FAILURE);
						}

						if(sigaction(SIGTERM, &act, NULL)==-1){
							perror("sigaction");
							exit(EXIT_FAILURE);
						}

						if(sigaction(SIGSEGV, &act, NULL)==-1){
							perror("sigaction");
							exit(EXIT_FAILURE);
						}

					    fprintf(stderr, "sender ID: %d parent ID:%d child ID:%d\n", getpid(), getppid(), sender);
					    senderWork(sender_fifo, mirror, input, found_str, logfile, buff_size, fp);					    						
					} 
					else{
					    receiver=fork();

					    if (receiver==0){

					    	//kanw setup ton handler gia ton receiver
					    	if(sigaction(SIGPIPE, &act, NULL)==-1){
								perror("sigaction");
								exit(EXIT_FAILURE);
							}

							if(sigaction(SIGTERM, &act, NULL)==-1){
								perror("sigaction");
								exit(EXIT_FAILURE);
							}

							if(sigaction(SIGSEGV, &act, NULL)==-1){
								perror("sigaction");
								exit(EXIT_FAILURE);
							}


					        fprintf(stderr,"receiver ID: %d parent ID:%d child ID:%d\n", getpid(), getppid(), receiver);
					        receiverWork(receiver_fifo, mirror, input, found_str, logfile, buff_size, fp);					    
					    } 
					    else{
					        printf("This is the parent ID: %d\n", getpid());
					        currSig_node->receiver=receiver;
							currSig_node->sender=sender;
							currSig_node->retry_count=0;						   

						        					   
						}  

					}

					
					if((*prevSig_node)!=NULL) (*prevSig_node)->next=currSig_node;
					(*prevSig_node)=currSig_node;
					currSig_node=currSig_node->next;
				}
				else{
					currBuck->arr[synced].status=status;
				}
			}
		}
		closedir (com_dir);


		//TWRA KANW ELEGXO GIA SIGNALS
		while(signaled){
			//prwta blockarw ta alla signals
			sigprocmask(SIG_SETMASK, &(act.sa_mask), NULL); 


			//sigint h sigquit
			if((signaled==3)||(signaled==4)){
				fprintf(stderr, "Deleting this client from common.\n");
				char to_delete[1024];
				strcpy(to_delete, common);
				strcat(to_delete,"/");
				strcat(to_delete,id_str);
				strcat(to_delete, ".id");
				if(unlink(to_delete)!=0){
					fprintf(stderr, "Can't delete %s.Exiting program.\n",to_delete);
					exit(1);
				}
				pid_t remover=fork();
				if(remover==0){
					char *args[]={"rm","-r", mirror, NULL}; 
		        	if(execvp(args[0],args)==-1) fprintf(stderr, "Child %d failed to remove directory %s\n", getpid(), mirror);
				}
				else{
					wpid = wait(&stat);
					fprintf(fp, "Client %d exiting.\n", id);
					fprintf(fp, "JUST_LEFT: %d\n",id);
					return 0;
				}
				signaled=0;
			}
			//sigusr2 -transfer complete
			else if(signaled==5){
				currSig_node=(*sigList);
				while(currSig_node!=NULL){
					if(currSig_node->sender==sigSender){
						fprintf(fp, "Transfer from client %d to client %d is complete.\n", id, currSig_node->id );
						break;
					}
					if(currSig_node->receiver==sigSender){
						fprintf(fp, "Transfer from client %d to client %d is complete.\n", currSig_node->id, id);
						break;
					}

					currSig_node=currSig_node->next;
				}

				signaled=0;				
			}

			//sigusr1
			else if(signaled==1){

				
		    	fprintf(fp,"Child %d sent SIGUSR.\n",sigSender);

		    	currSig_node=(*sigList);

		    	
		    	//tha vrw to telos ngia na desmevsw to neo signal
		    	while(currSig_node!=NULL){

		    		//an vrika to signal pou epsaxna kai tairiazei o sender tou signal me kapoio paidi, tote skotwnw to allo paidi

		    		if((currSig_node->sender==sigSender)||(currSig_node->receiver==sigSender)){
		    			kill(currSig_node->receiver, SIGTERM);
		    			kill(currSig_node->sender, SIGTERM);

		    			//ftiaxnw ta onomata twn fifo pali
	    				char sender_fifo[25], receiver_fifo[25], signaled_id[20];
	    				sprintf(signaled_id, "%d", currSig_node->id);

	    				strcpy(sender_fifo,common);
						strcat(sender_fifo,"/");
						strcat(sender_fifo,id_str);
						strcat(sender_fifo,"_to_");
						strcat(sender_fifo,signaled_id);


						strcpy(receiver_fifo,common);
						strcat(receiver_fifo,"/");
						strcat(receiver_fifo,signaled_id);
						strcat(receiver_fifo,"_to_");
						strcat(receiver_fifo,id_str);

						if(unlink(receiver_fifo)!=0) fprintf(stderr, "Child %d already deleted the fifo.\n", currSig_node->receiver );
						if(unlink(sender_fifo)!=0) fprintf(stderr, "Child %d already deleted the fifo.\n", currSig_node->sender );

						//an exoun ksemeinei (an kapoio paidia egine terminate kai de prolave na to svisei ta svinw)

		    			fprintf(fp, "Killed children %d  %d.\n",currSig_node->receiver, currSig_node->sender);
		    			if(currSig_node->retry_count<3){
		    				currSig_node->retry_count++;
		    				fprintf(fp, "	Retry attemp: %d\n",currSig_node->retry_count);

		    				

		    				//twra tha diagrapsw to mirror tou receiver
		    				char to_delete[1024];		    				
							strcpy(to_delete, mirror);
							strcat(to_delete,"/");							
							strcat(to_delete, signaled_id);

		    				pid_t remover=fork();
							if(remover==0){
								char *args[]={"rm","-rf", to_delete, NULL}; 
					        	execvp(args[0],args); 
							}
							else{

								wpid = wait(&stat);
								if((wpid==remover) && stat!=0) fprintf(stderr, "Oppposing client has already removed directory %s.\n", to_delete);

								currSig_node->sender=fork();

								if (currSig_node->sender==0){

									if(sigaction(SIGPIPE, &act, NULL)==-1){
										perror("sigaction");
										exit(EXIT_FAILURE);
									}

									if(sigaction(SIGTERM, &act, NULL)==-1){
										perror("sigaction");
										exit(EXIT_FAILURE);
									}

									if(sigaction(SIGSEGV, &act, NULL)==-1){
										perror("sigaction");
										exit(EXIT_FAILURE);
									}

								   // printf("sender ID: %d parent ID:%d child ID:%d\n", getpid(), getppid(), sender);
								    senderWork(sender_fifo, mirror, input, signaled_id, logfile, buff_size, fp);					    						
								} 
								else{
								    currSig_node->receiver=fork();

								    if (currSig_node->receiver==0){

								    	if(sigaction(SIGPIPE, &act, NULL)==-1){
											perror("sigaction");
											exit(EXIT_FAILURE);
										}

										if(sigaction(SIGTERM, &act, NULL)==-1){
											perror("sigaction");
											exit(EXIT_FAILURE);
										}

										if(sigaction(SIGSEGV, &act, NULL)==-1){
											perror("sigaction");
											exit(EXIT_FAILURE);
										}


								        //printf("receiver ID: %d parent ID:%d child ID:%d\n", getpid(), getppid(), receiver);
								        receiverWork(receiver_fifo, mirror, input, signaled_id, logfile, buff_size, fp);					    
								    } 
								    else{
								        //printf("This is the parent ID: %d\n", getpid());								        

									}
								}
								signaled=0;
				    			break;				    				
		    				}
		    			}
		    			else{
		    				fprintf(stderr, "Sorry, but you are out of retries.\n");
		    				fprintf(fp, "Transfer between clients %d and %d failed.\n", id, currSig_node->id );
		    				signaled=0;
		    				break;
		    			}
		    		}
		    				    	
		    		currSig_node=currSig_node->next;

		    	}
		    	signaled=0;		    	

		    }
		    sigprocmask(SIG_UNBLOCK, &(act.sa_mask), NULL); //kai edw kseblockarw
		}
		


		//elegxw ean exoun diagrafei arxeia gia na svisw ta antistoixa mirror dirs apo tous clients
		for(i=0;i<11;i++){
	        currBuck=(*idHash)[i];
	        k=0;
	        while(currBuck!=NULL){
	            for(j=0;j<currBuck->numOfEntries;j++){
	            	//printf("status %d  prev_status %d\n",currBuck->arr[j].status, prev_status );
	                if(currBuck->arr[j].status==(*prev_status)){
	                	fprintf(fp, "JUST_LEFT: %d\n", currBuck->arr[j].id);

	                	char to_delete[1024];
	                	char to_delete_id[20];
						strcpy(to_delete, mirror);
						strcat(to_delete,"/");
						sprintf(to_delete_id, "%d", currBuck->arr[j].id);
						strcat(to_delete,to_delete_id);

	                	pid_t remover=fork();
						if(remover==0){
							char *args[]={"rm","-r", to_delete, NULL}; 
				        	if(execvp(args[0],args)==-1) fprintf(stderr, "Child %d failed to remove directory %s\n", getpid(), to_delete);
						}
						else{
							wpid = wait(&stat);
							fprintf(fp, "Client %d deleted the copy of %d from its mirror.\n", id, currBuck->arr[j].id);
						}
	                }
	            }
	            currBuck=currBuck->next;
	            k++;
	        }
	    }

	    (*prev_status)=status;
	} 
	else{
		fprintf(fp, "Cannot open common directory. Exiting program.\n");
		return 0;
	}	
	fclose(fp);
	return 1;
}


int makeId(char* idFile){
	int i, id, j=0;
	char id_str[100];

	for(i=0;i<strlen(idFile);i++){
		if(idFile[i]=='.') break;
		id_str[j]=idFile[i];
		j++;
	}
	id_str[j]='\0';

	id=atoi(id_str);
	return id;
}
