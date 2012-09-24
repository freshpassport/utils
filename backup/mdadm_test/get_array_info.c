#include "../mdadm-3.2.3/mdadm.h"
#include "../mdadm-3.2.3/md_p.h"
#include "../mdadm-3.2.3/md_u.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define md_name "/dev/md1"

int get_array_info()
{
	mdu_array_info_t a;
	mdu_disk_info_t disk;
	mdu_version_t vers;
	mdu_bitmap_file_t bmf;
	int fd, i;
	
	fd = open(md_name, O_RDONLY);
	if (fd<0)
	{
		perror("open fd");
		return -1;
	}

	if (ioctl(fd, RAID_VERSION, &vers)>=0)
	{
		printf("Raid Version:\n");
		printf("  major: %d\n", vers.major);
		printf("  minor: %d\n", vers.minor);
		printf("  patch: %d\n", vers.patchlevel);
	}

	if ( (ioctl(fd, GET_BITMAP_FILE, &bmf)==0) && bmf.pathname[0] )
	{
		printf("\nbitmap file: %s\n", bmf.pathname);
	}

	if (ioctl(fd, GET_ARRAY_INFO, &a))
	{
		perror("ioctl");
		close(fd);
		return -2;
	}

	printf("\nGeneric constant information:\n");
	printf("   major_version: %d\n", a.major_version);
	printf("   minor_version: %d\n", a.minor_version);
	printf("   patch_version: %d\n", a.patch_version);
	printf("   ctime: %s", ctime(&a.ctime));
	printf("   level: %d\n", a.level);
	printf("   size: %d\n", a.size);
	printf("   nr_disks: %d\n", a.nr_disks);
	printf("   raid_disks: %d\n", a.raid_disks);
	printf("   md_minor: %d\n", a.md_minor);
	printf("   not_persistent: %d\n", a.not_persistent);

	printf("\nGeneric state information:\n");
	printf("   superblock update time: %s", ctime(&a.utime));
	printf("   state bits: %.8x\n", a.state);
	printf("   active_disks: %d\n", a.active_disks);
	printf("   working_disks: %d\n", a.working_disks);
	printf("   failed_disks: %d\n", a.failed_disks);
	printf("   spare_disks: %d\n", a.spare_disks);

	printf("\nPersionality information:\n");
	printf("   layout: %.8x\n", a.layout);
	printf("   chunk_size: %d\n", a.chunk_size);


	printf("\nDisk in this array:\n");
	//for (i=0; i<a.working_disks; i++)
	for (i=0; i<4; i++)
	{
		printf("   [%d] disk:\n", i);
		disk.number = i;
		if (ioctl(fd, GET_DISK_INFO, &disk)<0)
			continue;
		printf("      major: %d\n", disk.major);
		printf("      minor: %d\n", disk.minor);
		printf("      raid_disk: %d\n", disk.raid_disk);
		printf("      state: %d\n", disk.state);
	}
	close(fd);
}

int main()
{
	return get_array_info();
}

