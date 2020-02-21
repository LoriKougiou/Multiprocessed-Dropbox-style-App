#include "childWork.h"


//synarthsh pou ekteleneitai apo to sender paidi, simiourgei to fifo pou prepei, prospathei thn apostoli
//kai stelnei sima epitixias i apotixias ston patera

void senderWork(char* sender_fifo, char* mirror, char* input, char* found_str, char* logfile, int buff_size, FILE* fp){

	if(mkfifo(sender_fifo, 0666)==0){
    	fprintf(stderr, "FIFO pipe %s successfully created.\n", sender_fifo);
		if(writer(buff_size, sender_fifo, input, logfile)==0){
			if(unlink(sender_fifo)!=0){
				fprintf(stderr, "Failed to remove fifofile.Another process must have.\n");
			}
			kill(getppid(), SIGUSR1);
			exit(1);
		} 
		else{
			if(unlink(sender_fifo)!=0){
				fprintf(stderr, "Failed to remove fifofile.Another process must have.\n");
			}
			kill(getppid(), SIGUSR2);
			exit(0);
		}					
    } 
    else{
    	if(errno==EEXIST){
    		fprintf(stderr, "FIFO pipe %s has been already created by the oppposing client.\n", sender_fifo);
    		
    		if(writer(buff_size, sender_fifo, input, logfile)==0){
				kill(getppid(), SIGUSR1);
				exit(1);
			}	
			else{
				exit(0);
				kill(getppid(), SIGUSR2);
			}
			
	    }
    	else{
    		fprintf(fp, "FIFO pipe %s creation failed.Exiting program.\n", sender_fifo);
    		kill(getppid(), SIGUSR1);
    		exit(1);
    	}
    }
}



//synarthsh pou ekteleneitai apo to receiver paidi, dimiourgei to fifo pou prepei, prospathei thn lipsi
//kai stelnei sima epitixias i apotixias ston patera

void receiverWork(char* receiver_fifo, char* mirror, char* input, char* found_str, char* logfile, int buff_size, FILE* fp){

	if(mkfifo(receiver_fifo, 0666)==0){
    	fprintf(stderr, "FIFO pipe %s successfully created.\n", receiver_fifo);

    	char mirr_dir[1024];
    	snprintf(mirr_dir, sizeof(mirr_dir), "%s/%s", mirror, found_str);

		if(mkdir(mirr_dir, 0700)==0) fprintf(stderr, "Directory %s was successfully created.\n",mirr_dir);
		else{
			fprintf(stderr, "Directory %s creation failed.\n",mirr_dir);
			kill(getppid(), SIGUSR1);
			exit(1);
    	}

    	fp=fopen(logfile,"a");
		if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
		fprintf(fp, "%s    4096 bytes\n", mirr_dir);
		fclose(fp);

		if(reader(receiver_fifo, mirr_dir, logfile, buff_size)==0){
			if(unlink(receiver_fifo)!=0){
				fprintf(stderr, "Failed to remove fifofile.Another process must have.\n");
			}
			kill(getppid(), SIGUSR1);
			exit(1);
		}
		else{
			if(unlink(receiver_fifo)!=0){
				fprintf(stderr, "Failed to remove fifofile.Another process must have.\n");
			}
			kill(getppid(), SIGUSR2);
			exit(0);
		} 
    }
    else{
    	if(errno==EEXIST){
    		fprintf(stderr, "FIFO pipe %s has been already created by the oppposing client.\n", receiver_fifo);

    		char mirr_dir[1024];
    		snprintf(mirr_dir, sizeof(mirr_dir), "%s/%s", mirror, found_str);

    		if(mkdir(mirr_dir, 0700)==0) fprintf(stderr, "Directory %s was successfully created.\n",mirr_dir);
			else{
				fprintf(stderr, "Directory %s creation failed.\n",mirr_dir);
				kill(getppid(), SIGUSR1);
    			exit(1);
    		}

    		fp=fopen(logfile,"a");
			if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
			fprintf(fp, "%s    4096 bytes\n", mirr_dir);
			fclose(fp);

			if(reader(receiver_fifo, mirr_dir, logfile, buff_size)==0){
    			kill(getppid(), SIGUSR1);
    			exit(1);
    		}
			else{
				kill(getppid(), SIGUSR2);
				exit(0);
			}
    	}
    	else {
    		fprintf(fp, "FIFO pipe %s creation failed.Exiting Program.\n", receiver_fifo);
    		kill(getppid(), SIGUSR1);
    		exit(1);
    	}
	}
}