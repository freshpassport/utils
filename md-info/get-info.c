#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define __u32 uint32_t
#define __u8 uint8_t
#define __u64 uint64_t
#define __u16 uint16_t

struct mdp_superblock_1 {
	/* constant array information - 128 bytes */
	__u32	magic;		/* MD_SB_MAGIC: 0xa92b4efc - little endian */
	__u32	major_version;	/* 1 */
	__u32	feature_map;	/* 0 for now */
	__u32	pad0;		/* always set to 0 when writing */

	__u8	set_uuid[16];	/* user-space generated. */
	char	set_name[32];	/* set and interpreted by user-space */

	__u64	ctime;		/* lo 40 bits are seconds, top 24 are microseconds or 0*/
	__u32	level;		/* -4 (multipath), -1 (linear), 0,1,4,5 */
	__u32	layout;		/* only for raid5 currently */
	__u64	size;		/* used size of component devices, in 512byte sectors */

	__u32	chunksize;	/* in 512byte sectors */
	__u32	raid_disks;
	__u32	bitmap_offset;	/* sectors after start of superblock that bitmap starts
				 * NOTE: signed, so bitmap can be before superblock
				 * 				 * only meaningful of feature_map[0] is set.
				 * 				 				 */

	/* These are only valid with feature bit '4' */
	__u32	new_level;	/* new level we are reshaping to		*/
	__u64	reshape_position;	/* next address in array-space for reshape */
	__u32	delta_disks;	/* change in number of raid_disks		*/
	__u32	new_layout;	/* new layout					*/
	__u32	new_chunk;	/* new chunk size (bytes)			*/
	__u8	pad1[128-124];	/* set to 0 when written */

	/* constant this-device information - 64 bytes */
	__u64	data_offset;	/* sector start of data, often 0 */
	__u64	data_size;	/* sectors in this device that can be used for data */
	__u64	super_offset;	/* sector start of this superblock */
	__u64	recovery_offset;/* sectors before this offset (from data_offset) have been recovered */
	__u32	dev_number;	/* permanent identifier of this  device - not role in raid */
	__u32	cnt_corrected_read; /* number of read errors that were corrected by re-writing */
	__u8	device_uuid[16]; /* user-space setable, ignored by kernel */
	__u8    devflags;        /* per-device flags.  Only one defined...*/
#define WriteMostly1    1        /* mask for writemostly flag in above */
	__u8	pad2[64-57];	/* set to 0 when writing */

	/* array state information - 64 bytes */
	__u64	utime;		/* 40 bits second, 24 btes microseconds */
	__u64	events;		/* incremented when superblock updated */
	__u64	resync_offset;	/* data before this offset (from data_offset) known to be in sync */
	__u32	sb_csum;	/* checksum upto dev_roles[max_dev] */
	__u32	max_dev;	/* size of dev_roles[] array to consider */
	__u8	pad3[64-32];	/* set to 0 when writing */

	/* device state information. Indexed by dev_number.
	 * 	 * 2 bytes per device
	 * 	 	 * Note there are no per-device state flags. State information is rolled
	 * 	 	 	 * into the 'roles' value.  If a device is spare or faulty, then it doesn't
	 * 	 	 	 	 * have a meaningful role.
	 * 	 	 	 	 	 */
	__u16	dev_roles[0];	/* role in array, or 0xffff for a spare, or 0xfffe for faulty */
};

int main()
{
	int fd;
	struct mdp_superblock_1 info;
	size_t n;
	int i;
	char *p;

	fd = open("/dev/sdc", O_RDONLY);
	if (fd<0)
	{
		printf("open error!\n");
		return -1;
	}

	lseek(fd, 4096, SEEK_SET);
	n = read(fd, &info, sizeof(struct mdp_superblock_1));
	printf("n = %d, name = %s\n", n, info.set_name);

	close(fd);

	return 0;
}
