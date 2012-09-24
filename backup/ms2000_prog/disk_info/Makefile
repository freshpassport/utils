# File: Makefile
# Function: wis_mgr program makefile
# Features:
# Organization: WISDATA.
# Authors: wangtianxiu
# Version:
# Date: 11/07/2011
CC=$(CROSS_COMPILE)gcc
LIB=-lpthread
SRCS = $(wildcard *.c)
BUILD = $(patsubst %.c, %, $(SRCS))

.c:
	$(CC) -o $@ $< $(LIB) 
        
all: $(BUILD)
	cp -f $(BUILD) ../

clean:
	rm -f $(BUILD)







