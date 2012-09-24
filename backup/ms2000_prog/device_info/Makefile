# File: Makefile
# Function: wis_mgr program makefile
# Features:
# Organization: WISDATA.
# Authors: wangtianxiu
# Version:
# Date: 11/07/2011
CC=$(CROSS_COMPILE)gcc
SRCS=$(*.c)
TARGET1=wis_device
TARGET2=wis_mond

all: $(BUILD)
	$(CC) -o $(TARGET1) mon_interface.c wis_device.c -lpthread
	$(CC) -o $(TARGET2) mon_interface.c wis_monitor.c -lpthread
	cp -f $(TARGET1) $(TARGET2) ../

clean:
	rm -f ../$(TARGET1) ../$(TARGET2) *.o a.out $(TARGET1) $(TARGET2)

