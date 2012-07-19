#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/kdev_t.h>

#define DISK_ID_MAX 64
#define RAID_SLOT_MAX DISK_ID_MAX*UINT8_MAX
#define MAX_DISK 112

struct raid_info{
	char dev[PATH_MAX];
	char name[64];
	int level;
	int chunk_size;
	uint8_t state;
	uint8_t rebuild_percent;
	loff_t size;
	uint8_t total_devices;
	char *rdisks[DISK_ID_MAX];
	char slot_list[RAID_SLOT_MAX];
};

ssize_t wis_raid_info(const char *md_name, struct raid_info *info, size_t n);


