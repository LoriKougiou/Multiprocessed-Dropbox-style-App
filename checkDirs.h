#ifndef CHECKDIRS_H
#define CHECKDIRS_H

#include "hash.h"
#include "readwrite.h"
#include "sighandle.h"
#include "childWork.h"



int syncedWithId(bucket*** idHash, int id, bucket** toWrite);
void createIdFile(int client_id, char* common, char* logfile);
int fileExists(char *path);
int makeId(char* idFile);


void initDirCheck(char* input, char* common, char* mirror, char* logfile);
int checkCommon(char* common, char* logfile,char* input,char* mirror, bucket*** idHash, int maxBuckEnts, int id, int buff_size, int* prev_status, sigList_node** sigList, sigList_node** prevSig_node);




#endif
	