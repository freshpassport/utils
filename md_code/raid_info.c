#include "../mdadm/mdadm.h"
#include "../mdadm/md_p.h"
#include "../mdadm/md_u.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/kdev_t.h>
#include "raid_common.h"

#define DISK_ID_MAX 64
#define RAID_SLOT_MAX DISK_ID_MAX*UINT8_MAX
#define MAX_DISK 112

int main(int argc, char *argv[])
{
	char name[64];
	struct raid_info info;
	size_t n=1;
	if(argc <= 1)
	{
		printf("Please input the raid name to get info \n");
		return -1;
	}
	else
	{
		strcpy(name, argv[1]);
	}
	wis_raid_info(name, &info, n);
}

