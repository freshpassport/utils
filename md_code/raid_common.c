#include "../mdadm/mdadm.h"
#include "../mdadm/md_p.h"
#include "../mdadm/md_u.h"
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

struct devmap {
    int major, minor;
    char *name;
    struct devmap *next;
} *devlist = NULL;
int devlist_ready = 0;

int add_dev(const char *name, const struct stat *stb, int flag, struct FTW *s)
{
	struct stat st;
	if (S_ISLNK(stb->st_mode)) {
		if (stat(name, &st) != 0)
			return 0;
		stb = &st;
	}

	if ((stb->st_mode&S_IFMT)== S_IFBLK) {
		char *n = strdup(name);
		struct devmap *dm = malloc(sizeof(*dm));
		if (strncmp(n, "/dev/./", 7)==0)
			strcpy(n+4, name+6);
		if (dm) {
			dm->major = major(stb->st_rdev);
			dm->minor = minor(stb->st_rdev);
			dm->name = n;
			dm->next = devlist;
			devlist = dm;
		}
	}
	return 0;
}

#ifndef HAVE_NFTW
#ifdef HAVE_FTW
int add_dev_1(const char *name, const struct stat *stb, int flag)
{
	return add_dev(name, stb, flag, NULL);
}
int nftw(const char *path, int (*han)(const char *name, const struct stat *stb, int flag, struct FTW *s), int nopenfd, int flags)
{
	return ftw(path, add_dev_1, nopenfd);
}
#else
int nftw(const char *path, int (*han)(const char *name, const struct stat *stb, int flag, struct FTW *s), int nopenfd, int flags)
{
	return 0;
}
#endif /* HAVE_FTW */
#endif /* HAVE_NFTW */


char *map_dev(int major, int minor, int create)
{
	struct devmap *p;
	char *regular = NULL, *preferred=NULL;
	int did_check = 0;

	if (major == 0 && minor == 0)
		return NULL;

 retry:
	if (!devlist_ready) {
		char *dev = "/dev";
		struct stat stb;
		while(devlist) {
			struct devmap *d = devlist;
			devlist = d->next;
			free(d->name);
			free(d);
		}
		
		if (lstat(dev, &stb)==0 &&
		    S_ISLNK(stb.st_mode))
			dev = "/dev/.";
		nftw(dev, add_dev, 10, FTW_PHYS);
		devlist_ready=1;
		did_check = 1;
	}
	
	for (p=devlist; p; p=p->next)
		if (p->major == major &&
		    p->minor == minor) {
			if (strncmp(p->name, "/dev/md/",8) == 0) {
				if (preferred == NULL ||
				    strlen(p->name) < strlen(preferred))
					preferred = p->name;
			} else {
				if (regular == NULL ||
				    strlen(p->name) < strlen(regular))
					regular = p->name;
			}
		}
	if (!regular && !preferred && !did_check) {
		devlist_ready = 0;
		goto retry;
	}
	if (create && !regular && !preferred) {
		static char buf[30];
		snprintf(buf, sizeof(buf), "%d:%d", major, minor);
		regular = buf;
	}

	return preferred ? preferred : regular;
}

ssize_t wis_raid_info(const char *md_name, struct raid_info *info, size_t n)
{
	mdu_array_info_t array;
	mdu_disk_info_t *disks;
	mdu_version_t vers;
	mdu_bitmap_file_t bmf;
	int fd, d, next;
	
	fd = open(md_name, O_RDONLY);
	if (fd<0)
	{
		perror("open fd");
		return -1;
	}
	
	if (ioctl(fd, GET_ARRAY_INFO, &array))
	{
		perror("ioctl");
		close(fd);
		return -2;
	}
	
	strcpy(info->name, md_name);
	info->level = array.level;
	info->chunk_size = array.chunk_size;
	info->state = array.state;
	info->size = array.size;
	info->total_devices = array.nr_disks;
	
	printf("    raid name : %s \n", info->name);
	printf("        level : %d \n", info->level);
	printf("   chunk_size : %d \n", info->chunk_size);
	printf("        state : %d \n", info->state);
	printf("         size : %d \n", info->size);
	printf("total_devices : %d \n", info->total_devices);
	printf("   raid_disks : %d \n", array.raid_disks);
	printf("       rdisks : ");
	disks = malloc(MAX_DISK * sizeof(mdu_disk_info_t));
	for (d=0; d<MAX_DISK; d++)
	{
		disks[d].state = (1<<3);
		disks[d].major = disks[d].minor = 0;
		disks[d].number = disks[d].raid_disk = d;
	}
	
	next = array.raid_disks;
	for (d=0; d<MAX_DISK; d++)
	{
		mdu_disk_info_t disk;
		disk.number = d;
		if (ioctl(fd, GET_DISK_INFO, &disk)<0)
			continue;
		if (disk.major == 0 && disk.minor == 0)
			continue;
		
		if (disk.raid_disk >= 0 && disk.raid_disk < array.raid_disks)
			disks[disk.raid_disk] = disk;
		else if (next < MAX_DISK)
			disks[next++] = disk;
	}
	
	for (d=0; d<MAX_DISK; d++)
	{
		mdu_disk_info_t disk = disks[d];
		if (d >= array.raid_disks &&
			disk.major == 0 &&
			disk.minor == 0)
			continue;
		
		/* get the name by major and minor */
		char *dv = map_dev(disk.major, disk.minor, 0);
		info->rdisks[d] = dv;
		printf("%s ",  info->rdisks[d]);
	}
	printf("\n");
	free(disks);
	close(fd);
}


