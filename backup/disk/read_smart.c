#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define HDIO_DRIVE_CMD 0x031f

typedef unsigned char UINT8;

void dump_buff(UINT8 *buff, int len)
{
	int i;

	for (i=0;i<len;i++)
	{   
		if ((i>=32) && (i%32)==0)
			printf("\n");
		printf("%.2x ", (UINT8)(buff+i));
	}   
	printf("\n");
}

int main(int argc, char *argv[])
{
	int fd;
	unsigned char buff[516];

	if (argc<2)
		return 1;

	fd = open(argv[1], O_RDWR);
	if (fd<0)
		return 1;

	buff[0] = 0xb0;  // ATA_SMART_CMD
	buff[2] = 0xd0;  // ATA_SMART_READ_VALUES
	buff[3] = 1;

	if (ioctl(fd, HDIO_DRIVE_CMD, buff))
	{
		close(fd);
		return -1;
	}

	dump_buff(buff+4, 512);

	return close(fd);
}

