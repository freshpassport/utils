#include <stdio.h>
#include <stdint.h>

#define __u32 uint32_t
#define __u64 uint64_t

#define MD_SB_GENERIC_CONSTANT_WORDS	32
#define MD_SB_DESCRIPTOR_WORDS		32
#define MD_SB_GENERIC_STATE_WORDS	32
#define MD_SB_PERSONALITY_WORDS		64
#define MD_SB_DISKS			27
#define MD_SB_DISKS_WORDS		(MD_SB_DISKS*MD_SB_DESCRIPTOR_WORDS)
#define MD_SB_GENERIC_WORDS		(MD_SB_GENERIC_CONSTANT_WORDS + MD_SB_GENERIC_STATE_WORDS)
#define MD_SB_RESERVED_WORDS		(1024 - MD_SB_GENERIC_WORDS - MD_SB_PERSONALITY_WORDS - MD_SB_DISKS_WORDS - MD_SB_DESCRIPTOR_WORDS)

typedef struct mdp_device_descriptor_s {
	__u32 number;		/* 0 Device number in the entire set	      */
	__u32 major;		/* 1 Device major number		      */
	__u32 minor;		/* 2 Device minor number		      */
	__u32 raid_disk;	/* 3 The role of the device in the raid set   */
	__u32 state;		/* 4 Operational state			      */
	__u32 reserved[MD_SB_DESCRIPTOR_WORDS - 5];
} mdp_disk_t;

typedef struct mdp_superblock_s {
	/*
	 * 	 * Constant generic information
	 * 	 	 */
	__u32 md_magic;		/*  0 MD identifier 			      */
	__u32 major_version;	/*  1 major version to which the set conforms */
	__u32 minor_version;	/*  2 minor version ...			      */
	__u32 patch_version;	/*  3 patchlevel version ...		      */
	__u32 gvalid_words;	/*  4 Number of used words in this section    */
	__u32 set_uuid0;	/*  5 Raid set identifier		      */
	__u32 ctime;		/*  6 Creation time			      */
	__u32 level;		/*  7 Raid personality			      */
	__u32 size;		/*  8 Apparent size of each individual disk   */
	__u32 nr_disks;		/*  9 total disks in the raid set	      */
	__u32 raid_disks;	/* 10 disks in a fully functional raid set    */
	__u32 md_minor;		/* 11 preferred MD minor device number	      */
	__u32 not_persistent;	/* 12 does it have a persistent superblock    */
	__u32 set_uuid1;	/* 13 Raid set identifier #2		      */
	__u32 set_uuid2;	/* 14 Raid set identifier #3		      */
	__u32 set_uuid3;	/* 15 Raid set identifier #4		      */
	__u32 gstate_creserved[MD_SB_GENERIC_CONSTANT_WORDS - 16];

	/*
	 * 	 * Generic state information
	 * 	 	 */
	__u32 utime;		/*  0 Superblock update time		      */
	__u32 state;		/*  1 State bits (clean, ...)		      */
	__u32 active_disks;	/*  2 Number of currently active disks	      */
	__u32 working_disks;	/*  3 Number of working disks		      */
	__u32 failed_disks;	/*  4 Number of failed disks		      */
	__u32 spare_disks;	/*  5 Number of spare disks		      */
	__u32 sb_csum;		/*  6 checksum of the whole superblock        */
#if  __BYTE_ORDER ==  __BIG_ENDIAN
	__u32 events_hi;	/*  7 high-order of superblock update count   */
	__u32 events_lo;	/*  8 low-order of superblock update count    */
	__u32 cp_events_hi;	/*  9 high-order of checkpoint update count   */
	__u32 cp_events_lo;	/* 10 low-order of checkpoint update count    */
#else
	__u32 events_lo;	/*  7 low-order of superblock update count    */
	__u32 events_hi;	/*  8 high-order of superblock update count   */
	__u32 cp_events_lo;	/*  9 low-order of checkpoint update count    */
	__u32 cp_events_hi;	/* 10 high-order of checkpoint update count   */
#endif
	__u32 recovery_cp;	/* 11 recovery checkpoint sector count	      */
	/* There are only valid for minor_version > 90 */
	__u64 reshape_position;	/* 12,13 next address in array-space for reshape */
	__u32 new_level;	/* 14 new level we are reshaping to	      */
	__u32 delta_disks;	/* 15 change in number of raid_disks	      */
	__u32 new_layout;	/* 16 new layout			      */
	__u32 new_chunk;	/* 17 new chunk size (bytes)		      */
	__u32 gstate_sreserved[MD_SB_GENERIC_STATE_WORDS - 18];

	/*
	 * 	 * Personality information
	 * 	 	 */
	__u32 layout;		/*  0 the array's physical layout	      */
	__u32 chunk_size;	/*  1 chunk size in bytes		      */
	__u32 root_pv;		/*  2 LV root PV */
	__u32 root_block;	/*  3 LV root block */
	__u32 pstate_reserved[MD_SB_PERSONALITY_WORDS - 4];

	/*
	 * 	 * Disks information
	 * 	 	 */
	mdp_disk_t disks[MD_SB_DISKS];

	/*
	 * 	 * Reserved
	 * 	 	 */
	__u32 reserved[MD_SB_RESERVED_WORDS];

	/*
	 * 	 * Active descriptor
	 * 	 	 */
	mdp_disk_t this_disk;

} mdp_super_t;

int main()
{
	printf("sizeof(mdp_super_t) = %d\n", sizeof(mdp_super_t));
}

