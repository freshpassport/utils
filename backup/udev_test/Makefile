CROSS_COMPILE=mips64-octeon-linux-gnu-
CC=$(CROSS_COMPILE)gcc
#CINCLUDE=-I/home/gonglei/udev-166/libudev/
#CLIBS=-L/home/gonglei/udev-166/libudev/.libs/

CINCLUDE=-I/root/gl/udev_release/udev-166/libudev/
CLIBS=-L/root/gl/udev_release/udev-166/libudev/.libs/

all:
	$(CC) $(CLIBS) $(CINCLUDE) -g -o udev_enum udev_enum.c -ludev 

clean:
	rm -fr a.out core udev_enum

