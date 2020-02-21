#include "hash.h"


//synarthseis gia to hashtable twn ids
int hash_func(int key){
	return key%11;
}

//insert to hashtable
void insert_key(bucket*** idHash, int key, int maxBuckEnts, int stat){
	bucket* currBuck=NULL;
	int i, hash_val, numOfEntries;

	if((*idHash)==NULL){
		(*idHash)=malloc(11*sizeof(bucket**));
		if((*idHash)==NULL){
			fprintf(stderr, "Malloc failed.\n");
			exit(1);
		}
		for(i=0;i<11;i++){
	        (*idHash)[i]=NULL;
	    }
	}

	hash_val=hash_func(key);
	if((*idHash)[hash_val]==NULL){
		(*idHash)[hash_val]=malloc(sizeof(bucket));
		if((*idHash)[hash_val]==NULL){
	        fprintf(stderr, "Malloc failed.\n");
	        exit(1);
	    }
	    (*idHash)[hash_val]->arr=malloc(maxBuckEnts*sizeof(b_entry));
	    if((*idHash)[hash_val]->arr==NULL){
	        fprintf(stderr, "Malloc failed.\n");
	        exit(1);
	    }
	    (*idHash)[hash_val]->numOfEntries=0;
	    (*idHash)[hash_val]->next=NULL;

	    numOfEntries=(*idHash)[hash_val]->numOfEntries;

		(*idHash)[hash_val]->arr[numOfEntries].id=key;
		(*idHash)[hash_val]->arr[numOfEntries].status=stat;
	    (*idHash)[hash_val]->numOfEntries++;

	}
	else{ //an auto to hash value exei kai allous user mesa			
		currBuck=(*idHash)[hash_val];
		while(currBuck!=NULL){
			currBuck=currBuck->next;
		}
		if(currBuck->numOfEntries<maxBuckEnts){ //an o user xwraei sto bucket

			numOfEntries=currBuck->numOfEntries;
			currBuck->arr[numOfEntries].id=key;
			currBuck->arr[numOfEntries].status=stat;
		    currBuck->numOfEntries++;
		}
		else if(currBuck->numOfEntries==maxBuckEnts){ //ean o user den xwraei so bucket

			//ftiaxnw kainourgio bucket
			currBuck->next=malloc(sizeof(bucket));
			if(currBuck->next==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    currBuck=currBuck->next;
		    currBuck->arr=malloc(maxBuckEnts*sizeof(b_entry));
		    if(currBuck->arr==NULL){
		        fprintf(stderr, "Malloc failed.\n");
		        exit(1);
		    }
		    currBuck->numOfEntries=0;
		    currBuck->next=NULL;

		    numOfEntries=currBuck->numOfEntries;
			currBuck->arr[numOfEntries].id=key;
			currBuck->arr[numOfEntries].status=stat;
		    currBuck->numOfEntries++;
		}
		else{
			fprintf(stderr, "Something's wrong\n");
		}
	}
}

//apeleutherwnei ti mnhmh th mnhmh pou exei desmeftei gia ena hashtable
void free_hashTable(bucket** hashTable){
	int i;
	bucket* currBuck=NULL, *tempBuck=NULL;

	for(i=0;i<11;i++){
		if(hashTable[i]!=NULL){
			currBuck=hashTable[i];
			while(currBuck!=NULL){

				tempBuck=currBuck->next;
				free(currBuck->arr);				
				free(currBuck);
				currBuck=tempBuck;
			}
		}
	}
	free(hashTable);
}