CROSS_COMPLE=
CC=$(CROSS_COMPILE)gcc
INCLUDEDIR=-I.

all: raid_info raid_create raid_delete
raid_delete:raid_delete.o raid_common.o
	$(CC) -o raid_delete raid_delete.o raid_common.o

raid_delete.o:raid_delete.c raid_common.h
	$(CC) $(INCLUDEDIR)	-c raid_delete.c

raid_common.o: raid_common.c raid_common.h
	$(CC) $(INCLUDEDIR)	-c raid_common.c

raid_info:raid_info.o raid_common.o
	$(CC) -o raid_info raid_info.o raid_common.o

raid_info.o:raid_info.c raid_common.h
	$(CC) $(INCLUDEDIR)	-c raid_info.c

raid_create:raid_create.o
	$(CC) -o raid_create raid_create.o
	
raid_create.o:raid_create.c

clean:
	rm -fr core raid_delete raid_common raid_create raid_info *.o

