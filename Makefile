CC = gcc
CFLAGS  = -g3 -Wall
LIBS += -lm


mirror_client:  main.o checkDirs.o hash.o readwrite.o dirManagement.o childWork.o sighandle.o
	$(CC) $(CFLAGS) -o mirror_client main.o checkDirs.o hash.o readwrite.o dirManagement.o childWork.o sighandle.o $(LIBS)

functions.o:  checkDirs.c checkDirs.h
	$(CC) $(CFLAGS) -c checkDirs.c

hash.o:  hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c	

readwrite.o:  readwrite.c readwrite.h
	$(CC) $(CFLAGS) -c readwrite.c

dirManagement.o: dirManagement.c dirManagement.h
	$(CC) $(CFLAGS) -c dirManagement.c

childWork.o:  childWork.c childWork.h
	$(CC) $(CFLAGS) -c childWork.c

sighandle.o:  sighandle.c sighandle.h
	$(CC) $(CFLAGS) -c sighandle.c


clean: 
	-rm -f *.o 
	-rm -f mirror_client
