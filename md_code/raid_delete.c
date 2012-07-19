#include "../mdadm/mdadm.h"
#include "../mdadm/md_p.h"
#include "../mdadm/md_u.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "raid_common.h"

/* #define md_name "/dev/md1" */
/*
	mdadm-stop-raid
	mdadm --zero-superblock /dev/sdx
*/
ssize_t wis_raid_delete(const char *name)
{
	int fd, tmp;
	char cmd[64], line[300];
	struct raid_info info;
	
	fd = open(name, O_RDONLY);
	if (fd < 0)
	{
		perror("open fd");
		return -1;
	}
	
	/* get disk in fd */
	wis_raid_info(name, &info, 1);
	
	/* Stop md */
	if(ioctl(fd, STOP_ARRAY, NULL))
	{
		perror("ioctl");
		close(fd);
		return -2;
	}
	
	for(tmp=0; tmp<info.total_devices; tmp++)
	{
		printf("disk in raid is %s \n", info.rdisks[tmp]);
		/* --zero-superblock /dev/sdx */
		char *disk = info.rdisks[tmp];
		sprintf(cmd, "mdadm --zero-superblock %s 2>&1", disk);
		FILE *stream = popen(cmd, "r");
		if(!stream)
		{
			printf("mdadm --zero-superblock %s is ERROR \n", disk);
		}
		else
		{
			if(fgets(line, 100, stream) != NULL)
			{
				printf("the result is %s \n", line);
			}
		}
		pclose(stream);
	}
}

int main(int argc, char *argv[])
{
	char name[64];
	if(argc <= 1)
	{
		printf("Please input the raid name to delete \n");
		return -1;
	}
	else
	{
		strcpy(name, argv[1]);
	}
	return wis_raid_delete(name);
}

