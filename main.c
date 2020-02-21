#include "checkDirs.h"
#define BUCKSIZE 100

int main(int argc, char** argv){

	int id, buff_size, i, loops, prev_status=0;
	char common[50], input[50], mirror[50], logfile[50];
	bucket** idHash=NULL;
	int maxBuckEnts;
	sigList_node* sigList=NULL, *prevSig_node=NULL;

	maxBuckEnts=(BUCKSIZE-sizeof(int)-sizeof(bucket*))/sizeof(int);


	//argument parsing
	if(argc<13){
	    fprintf(stderr,"Invalid number of arguments.Exiting the program.\n");
	    exit(1);
	}
	if(strcmp(argv[0],"./mirror_client")!=0){
	    fprintf(stderr,"Invalid name of program.Exiting the program.\n");
	    exit(1);
	}

	if(argc==13){ //se auth th periptwsh exw ola ta arguments gia ta files 
	    for(i=0;i<argc;i++){
	        if((i!=0)&&(i<argc-1)){
	            if(strcmp(argv[i], "-n")==0) id=atoi(argv[i+1]);
	            else if(strcmp(argv[i], "-b")==0) buff_size=atoi(argv[i+1]);
	            else if(strcmp(argv[i], "-c")==0) strcpy(common, argv[i+1]);
	            else if(strcmp(argv[i], "-i")==0) strcpy(input, argv[i+1]); 
	            else if(strcmp(argv[i], "-m")==0) strcpy(mirror, argv[i+1]);  
	            else if(strcmp(argv[i], "-l")==0) strcpy(logfile, argv[i+1]); 
	            else{
	                //fprintf(stderr, "Invalid type of argument  %s\n", argv[i]);
	                //exit(1);
	            }     
	        }
	    }
	}              
	else{
	    fprintf(stderr, "Invalid number of arguments.Exiting the program.\n");
	    exit(1);
	}

	//kanw tous arxikous elegxous gia ta dierectories
	initDirCheck(input, common, mirror, logfile);

	//twra tha gprasw to arxeio me to anagnwristiko tou client sto common
	createIdFile(id, common, logfile);
	insert_key(&idHash, id, maxBuckEnts,0);

	//twra tha tsekarw an o common exei hdh mesa allous client 
	loops=0; //auti i mwtavliti tha metraei thn periodo, ptan oloklirwthoun 10 loopes thereitai h periodos T kai prepei na ksanaksei elegxo sto common
	while(1){
		loops++;
		//printf("loop #%d\n", loops);
		if(loops%10==0) {
			//fprintf(stderr, "Checking common\n");
			if(checkCommon(common, logfile, input, mirror, &idHash, maxBuckEnts, id, buff_size,&prev_status, &sigList, &prevSig_node)==0) break;
		}
	}

  	if(sigList!=NULL) free_SigList(sigList);
  	free_hashTable(idHash);
	return 0;
}
