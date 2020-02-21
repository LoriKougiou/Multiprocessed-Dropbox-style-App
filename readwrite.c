#include "readwrite.h"

//synarthsh reader, ean den exei lavei input apo to fifo pipe ta prwta 30 secs, epistrefei kai to paidi stelnei sigusr. 
//kai se opoiodipote lathos epistrefei kai to paidi stelnei sigusr lathous
int reader(char* fifofile, char* mirror, char* logfile, int buff_size){

	int readfd, j, i, count, n, isFile, read_bytes, to_end, rc, byteCounter=0, fileCounter=0;
	FILE* fp;
	unsigned short int filename_len;
	unsigned int file_len;
	struct stat sb;

	char new_file[1024], new_dir[1024], buffer[buff_size], *filename=NULL, name[1024];;

	struct pollfd fdArr[1];


	if ((readfd=open(fifofile, O_RDONLY, 0 )) < 0){
		fprintf(stderr, "Can't open pipe %s for reading. Returning\n", fifofile);
		return 0;
	}

	//arxikopoiw tis parametrous tou poll

	fdArr[0].fd=readfd;
	fdArr[0].events=POLLIN;

	//anamenw na grapsei kapoios sto pipe, i a kseperastun ta 30 sec

	rc=poll(fdArr, 1, 30);
	if(rc ==0){
		printf("30 seconds passed, and still nothing!\n");
		return 0;
	}
	else if((rc==1)&&(fdArr[0].revents==POLLIN)&&(fdArr[0].fd==readfd)){

		while(1){	
			
			fileCounter++;

			//mhkos onomatos 
			if((n=read(readfd, &filename_len, sizeof(unsigned short int))) >0){
				if(filename_len==0) to_end=1;
				else to_end=0;
		      
				if(to_end){
					fileCounter--;
					close(readfd);
					break;
				}
			} 
			else{
				fprintf(stderr, "Reader: filename length error. Returning\n");
				fp=fopen(logfile,"a");
				if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
				fprintf(fp, "BYTES_READ: %d  bytes\n", byteCounter);
				fprintf(fp, "FILES_READ: %d\n", fileCounter);
				fclose(fp);

				close(readfd);
				return 0;
			}

			//lamvanw to onoma tou arxeiou
			
			filename=malloc((filename_len+1)*sizeof(char));
			if(filename==NULL){
				fprintf(stderr, "Malloc failed.\n");
				exit(1);
			}

			if ((n=read(readfd, filename, filename_len)) >0){

				filename[n]='\0';
				if(stat(filename, &sb) == 0){
					if(S_ISDIR(sb.st_mode)){
						
						i=0;
						count=0; 
						j=0;

						memset(name, 0, 1024);

						for(i=0;i<filename_len;i++){
							if((count<2)&&(filename[i]=='/')) count++;
							if(count==2){
								name[j]=filename[i];
								j++;
							}
						}
						name[j]='\0';


						if(name[0]=='/') snprintf(new_dir, strlen(mirror)+strlen(name)+1, "%s%s", mirror, name);
						else snprintf(new_dir, strlen(mirror)+strlen(name)+1, "%s/%s", mirror, name);

						if(strcmp(new_dir, mirror)!=0){

							if(mkdir(new_dir, 0700)!=0){
								fprintf(stderr, "Directory %s creation failed.Returning.\n",new_dir);
								free(filename);
								close(readfd);

								fp=fopen(logfile,"a");
								if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
								fprintf(fp, "FILES_READ: %d\n", fileCounter);
								fclose(fp);

								return 0;
							}
							isFile=0;
						}
						else{
							isFile=-1; //deixnei oti prokeitai gia to input dir kai ousiastika iparxei idi
							byteCounter+=4096;
						}

						
					}
					else if(S_ISREG(sb.st_mode)){

						i=0;
						count=0; 
						j=0;				

						
						memset(name, 0, 1024);

						for(i=0;i<filename_len;i++){
							if((count<2)&&(filename[i]=='/')) count++;
							if(count==2){
								name[j]=filename[i];
								j++;
							}
						}
						name[j]='\0';


						if(name[0]=='/') snprintf(new_file, strlen(mirror)+strlen(name)+1, "%s%s", mirror, name);
						else snprintf(new_file, strlen(mirror)+strlen(name)+1, "%s/%s", mirror, name);


						fp = fopen(new_file, "w+");
						if(fp==NULL){
							fprintf(stderr, "Creation of %s failed.Returning.\n",new_file);
							close(readfd);
							free(filename);

							fp=fopen(logfile,"a");
							if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
							fprintf(fp, "FILES_READ: %d\n", fileCounter);
							fclose(fp);

							return 0;
						}

						isFile=1;
						fclose(fp);
					}
				}
				else{
					fprintf(stderr, "Stat for %s failed.Returning.\n", filename);
					free(filename);
					close(readfd);
					fp=fopen(logfile,"a");
					if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
					fprintf(fp, "FILES_READ: %d\n", fileCounter);
					fclose(fp);

					return 0;
				}

			}
			else{
				fprintf(stderr, "Reader: filename error. Returning\n");
				free(filename);
				close(readfd);

				fp=fopen(logfile,"a");
				if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
				fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
				fprintf(fp, "FILES_READ: %d\n", fileCounter);
				fclose(fp);

				return 0;
			}

			free(filename);	
			
			//mhkos arxeiou
			if((n=read(readfd, &file_len, sizeof(unsigned int))) <=0){
				fprintf(stderr, "Reader: file length error. Returning\n");
				close(readfd);

				fp=fopen(logfile,"a");
				if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
				fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
				fprintf(fp, "FILES_READ: %d\n", fileCounter);
				fclose(fp);

				return 0;
			} 

			//lamvanw to arxeio 
			if(isFile==1){

				fp = fopen(new_file, "w+");
				if(fp==NULL){
					fprintf(stderr, "Can't open %s.\n", new_file);
					close(readfd);

					fp=fopen(logfile,"a");
					if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
					fprintf(fp, "FILES_READ: %d\n", fileCounter);
					fclose(fp);

					return 0;
				}
				

				read_bytes=0;
				while(read_bytes<file_len){

					memset(buffer, 0, buff_size);

					if(file_len-read_bytes >= buff_size-1){

						if((n=read(readfd, buffer, buff_size-1)) > 0){
							buffer[n]='\0';
							fprintf(fp, "%s", buffer);

							read_bytes+=n;
							byteCounter+=n;
						}
						else{
							fprintf(stderr, "Reader: textfile chunck error. Returning\n");
							close(readfd);

							fp=fopen(logfile,"a");
							if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
							fprintf(fp, "FILES_READ: %d\n", fileCounter);
							fclose(fp);

							return 0;
						}
					}
					else{

						if((n=read(readfd, buffer, file_len-read_bytes)) > 0){
							buffer[n]='\0';
							fprintf(fp, "%s", buffer);

							read_bytes+=n;
							byteCounter+=n;
						}
						else{
							fprintf(stderr, "Reader: textfile chunck error. Returning\n");
							close(readfd);

							fp=fopen(logfile,"a");
							if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
							fprintf(fp, "FILES_READ: %d\n", fileCounter);
							fclose(fp);

							return 0;
						}
					}
				}
				

				fclose(fp);

				fp=fopen(logfile,"a");
				if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
				fprintf(fp, "%s    %d bytes\n", new_file, file_len);
				fclose(fp);

			}
			else if(isFile==0){

				fp=fopen(logfile,"a");
				if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
				byteCounter+=file_len;
				fprintf(fp, "%s    %d bytes\n", new_dir, file_len);
				fclose(fp);
				
			}
			
		}
	}
	
	fp=fopen(logfile,"a");
	if(fp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
	fprintf(fp, "BYTES_READ: %d bytes\n", byteCounter);
	fprintf(fp, "FILES_READ: %d\n", fileCounter);
	fclose(fp);

	fprintf(stderr, "Reading complete.\n");
	return 1;
}


//synarthsh writer, se opoiodipote lathos epistrefei kai to paidi stelnei sigusr lathos ston patera
int writer(int buff_size, char* fifofile, char* input, char* logfile){
	int  i, writefd, rootInpassed=0, byteCounter=0, fileCounter=0;;
	FILE* fp, *logfp;
	char buffer[buff_size], c, filename[1024];
	unsigned short int filename_len;
	unsigned int file_len;
	struct stat sb;
	list_node* curr_node=NULL;
	DIR *dir;
    struct dirent *curr_file;
    firstLevel_node* listOfpaths, *currList;


	//diavazw tin ierarxia fakelwn kai tin apothikevw se mia lista
	listOfpaths=traverseDirs(input, buff_size);
	currList=listOfpaths;

	memset(buffer, 0, buff_size);

	if ((writefd=open(fifofile, O_WRONLY))  < 0){
		fprintf(stderr, "Can't open pipe %s for writing. Returning\n", fifofile);
		return 0;
	}


	while(currList!=NULL){
		curr_node=currList->start;

		//metavliti pou deixnei poses fores pernaw apo to input dir
		rootInpassed++;
		while(curr_node!=NULL){

			//pernaw to input dir kai ta text files tou mono mia fora
			if(strcmp(curr_node->directory, input)==0){
				if(rootInpassed==1){
					
					fileCounter++;

					//mhkos onomatos 
					filename_len=strlen(curr_node->directory);
					if (write(writefd, &filename_len, sizeof(unsigned short int)) != sizeof(unsigned short int)){
						fprintf(stderr, "Writer: filename length error.Returning\n");
						close(writefd);

						logfp=fopen(logfile,"a");
						if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
						fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
						fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
						fclose(logfp);

						return 0;
					}

					//stelnw to onoma tou arxeiou

					memset(filename,0,sizeof(filename));
					strcpy(filename,curr_node->directory);
					if (write(writefd, filename, filename_len) != filename_len){
						fprintf(stderr, "Writer: filename error.Returning\n");
						close(writefd);

						logfp=fopen(logfile,"a");
						if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
						fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
						fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
						fclose(logfp);

						return 0;
					}	

					
					//mhkos arxeiou
					if (stat(curr_node->directory, &sb) == 0) file_len=sb.st_size;
					if (write(writefd, &file_len, sizeof(unsigned int)) != sizeof(unsigned int)){
						fprintf(stderr, "Writer: file length error.Returning\n");
						close(writefd);

						logfp=fopen(logfile,"a");
						if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
						fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
						fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
						fclose(logfp);

						return 0;
					}

					byteCounter+=file_len;

					//Twra tha grapsw ta arxeia tou directory

					if (!(dir = opendir(curr_node->directory))){
						fprintf(stderr, "Can't open directory %s.Returning\n", curr_node->directory);
						close(writefd);

						logfp=fopen(logfile,"a");
						if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
						fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
						fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
						fclose(logfp);

						return 0;
					}

			    	while ((curr_file=readdir(dir))!=NULL){

				    	if(curr_file->d_type==DT_REG){

				    		fileCounter++;

							memset(filename,0,sizeof(filename));
				    		snprintf(filename, sizeof(filename), "%s/%s", curr_node->directory, curr_file->d_name);

				    		//mhkos onomatos 
							filename_len=strlen(filename);
							if (write(writefd, &filename_len, sizeof(unsigned short int)) != sizeof(unsigned short int)){
								fprintf(stderr, "Writer: filename length error.Returning\n");
								close(writefd);

								logfp=fopen(logfile,"a");
								if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
								fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
								fclose(logfp);

								return 0;
							}

							//stelnw to onoma tou arxeiou
							if (write(writefd, filename, filename_len) != filename_len){
								fprintf(stderr, "Writer: filename error.Returning\n");
								close(writefd);

								logfp=fopen(logfile,"a");
								if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
								fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
								fclose(logfp);

								return 0;
							}	
							
							//mhkos arxeiou
							if (stat(filename, &sb) == 0) file_len=sb.st_size;
							else{
								fprintf(stderr, "Stat failed for %s.Returning\n", buffer);
								close(writefd);
								
								logfp=fopen(logfile,"a");
								if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
								fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
								fclose(logfp);

								return 0;
							}
							if (write(writefd, &file_len, sizeof(unsigned int)) != sizeof(unsigned int)){
								fprintf(stderr, "Writer: file length error.Returning\n");
								close(writefd);

								logfp=fopen(logfile,"a");
								if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
								fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
								fclose(logfp);

								return 0;
							}	

							//stelnw to arxeio 
							memset(buffer, 0, buff_size);

							fp=fopen(filename, "r");	
							c=fgetc(fp);


							while(c!=EOF){

								for(i=0;i<buff_size-1;i++){
									if(c==EOF) break;
									buffer[i]=c;
									c=fgetc(fp);
								}

								buffer[i]='\0';
								if (write(writefd, buffer, strlen(buffer)) != strlen(buffer)){
									fprintf(stderr, "Writer: file chunck error.Returning\n");
									close(writefd);

									logfp=fopen(logfile,"a");
									if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
									fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
									fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
									fclose(logfp);

									return 0;
								}

								byteCounter+=strlen(buffer);
								memset(buffer, 0, buff_size);	
							}
							fclose(fp);
				    	}               
			    	}

			    	closedir(dir);

				}
			}
			else{
				
				fileCounter++;

				//mhkos onomatos 
				filename_len=strlen(curr_node->directory);
				if (write(writefd, &filename_len, sizeof(unsigned short int)) != sizeof(unsigned short int)){
					fprintf(stderr, "Writer: filename length error.Returning\n");
					close(writefd);

					logfp=fopen(logfile,"a");
					if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
					fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
					fclose(logfp);

					return 0;
				}

				//stelnw to onoma tou arxeiou

				memset(filename,0,sizeof(filename));
				strcpy(filename,curr_node->directory);
				if (write(writefd, filename, filename_len) != filename_len){
					fprintf(stderr, "Writer: filename error.Returning\n");
					close(writefd);

					logfp=fopen(logfile,"a");
					if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
					fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
					fclose(logfp);

					return 0;
				}	

				
				//mhkos arxeiou
				if (stat(curr_node->directory, &sb) == 0) file_len=sb.st_size;
				if (write(writefd, &file_len, sizeof(unsigned int)) != sizeof(unsigned int)){
					fprintf(stderr, "Writer: file length error.Returning\n");
					close(writefd);

					logfp=fopen(logfile,"a");
					if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
					fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
					fclose(logfp);

					return 0;
				}

				byteCounter+=file_len;

				//Twra tha grapsw ta arxeia tou directory

				if (!(dir = opendir(curr_node->directory))){
					fprintf(stderr, "Can't open directory %s.Returning\n", curr_node->directory);
					close(writefd);

					logfp=fopen(logfile,"a");
					if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
					fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
					fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
					fclose(logfp);

					return 0;
				}

		    	while ((curr_file=readdir(dir))!=NULL){

			    	if(curr_file->d_type==DT_REG){

			    		fileCounter++;

						memset(filename,0,sizeof(filename));
			    		snprintf(filename, sizeof(filename), "%s/%s", curr_node->directory, curr_file->d_name);

			    		//mhkos onomatos 
						filename_len=strlen(filename);
						if (write(writefd, &filename_len, sizeof(unsigned short int)) != sizeof(unsigned short int)){
							fprintf(stderr, "Writer: filename length error.Returning\n");
							close(writefd);

							logfp=fopen(logfile,"a");
							if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
							fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
							fclose(logfp);

							return 0;
						}

						//stelnw to onoma tou arxeiou
						if (write(writefd, filename, filename_len) != filename_len){
							fprintf(stderr, "Writer: filename error.Returning\n");
							close(writefd);

							logfp=fopen(logfile,"a");
							if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
							fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
							fclose(logfp);

							return 0;
						}	
						
						//mhkos arxeiou
						if (stat(filename, &sb) == 0) file_len=sb.st_size;
						else{
							fprintf(stderr, "Stat failed for %s.Returning\n", buffer);
							close(writefd);

							logfp=fopen(logfile,"a");
							if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
							fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
							fclose(logfp);

							return 0;
						}
						if (write(writefd, &file_len, sizeof(unsigned int)) != sizeof(unsigned int)){
							fprintf(stderr, "Writer: file length error.Returning\n");
							close(writefd);

							logfp=fopen(logfile,"a");
							if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
							fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
							fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
							fclose(logfp);

							return 0;
						}	

						//stelnw to arxeio 
						memset(buffer, 0, buff_size);

						fp=fopen(filename, "r");	
						c=fgetc(fp);


						while(c!=EOF){

							for(i=0;i<buff_size-1;i++){
								if(c==EOF) break;
								buffer[i]=c;
								c=fgetc(fp);
							}

							buffer[i]='\0';
							if (write(writefd, buffer, strlen(buffer)) != strlen(buffer)){
								fprintf(stderr, "Writer: file chunck error.Returning\n");
								close(writefd);

								logfp=fopen(logfile,"a");
								if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
								fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
								fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
								fclose(logfp);

								return 0;
							}

							byteCounter+=strlen(buffer);
							memset(buffer, 0, buff_size);	
						}
						fclose(fp);
			    	}               
		    	}

		    	closedir(dir);

			}
			
	    	curr_node=curr_node->next;
		}
		currList=currList->next;
	}
	
	unsigned short int to_end=0;
	if (write(writefd, &to_end, sizeof(unsigned short int)) != sizeof( unsigned short int)){
		fprintf(stderr, "Writer: error sending SUCCESS bytes\n");
		close(writefd);

		logfp=fopen(logfile,"a");
		if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
		fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
		fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
		fclose(logfp);

		return 0;
	}

	fprintf(stderr, "Writing complete.\n");
	freeFLDirList(listOfpaths);
	close(writefd);

	logfp=fopen(logfile,"a");
	if(logfp==NULL) fprintf(stderr, "Can't open %s.\n",logfile);
	fprintf(logfp, "BYTES_WRITTEN: %d bytes\n", byteCounter);
	fprintf(logfp, "FILES_WRITTEN: %d\n", fileCounter);
	fclose(logfp);

	return 1;
}
