#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <scsi/sg.h>
#include <linux/types.h>

#include "hdparm.h"
#define SG_CHECK_CONDITION 0x02
#define SG_DRIVER_SENSE 0x08
#define SG_ATA_16 0x85
#define SG_ATA_16_LEN 16
#define SG_ATA_LBA48 1
#define SG_ATA_PROTO_NON_DATA ( 3 1)
#define SG_ATA_PROTO_PIO_IN ( 4 1)
#define SG_ATA_PROTO_PIO_OUT ( 5 1)
#define SG_ATA_PROTO_DMA ( 6 1)
#define SG_ATA_PROTO_UDMA_IN (11 1) /* not yet supported in libata */
#define SG_ATA_PROTO_UDMA_OUT (12 1) /* not yet supported in libata */
#define ATA_USING_LBA (1 6)

enum {
	ATA_OP_CHECKPOWERMODE1 = 0xe5,
	ATA_OP_CHECKPOWERMODE2 = 0x98,
	ATA_OP_DOORLOCK = 0xde,
	ATA_OP_DOORUNLOCK = 0xdf,
	ATA_OP_FLUSHCACHE = 0xe7,
	ATA_OP_FLUSHCACHE_EXT = 0xea,
	ATA_OP_IDENTIFY = 0xec,
	ATA_OP_PIDENTIFY = 0xa1,
	ATA_OP_SECURITY_DISABLE = 0xf6,
	ATA_OP_SECURITY_ERASE_PREPARE = 0xf3,
	ATA_OP_SECURITY_ERASE_UNIT = 0xf4,
	ATA_OP_SECURITY_FREEZE_LOCK = 0xf5,
	ATA_OP_SECURITY_SET_PASS = 0xf1,
	ATA_OP_SECURITY_UNLOCK = 0xf2,
	ATA_OP_SETFEATURES = 0xef,
	ATA_OP_SETIDLE1 = 0xe3,
	ATA_OP_SETIDLE2 = 0x97,
	ATA_OP_SLEEPNOW1 = 0xe5,
	ATA_OP_SLEEPNOW2 = 0x99,
	ATA_OP_SMART = 0xb0,
	ATA_OP_STANDBYNOW1 = 0xe0,
	ATA_OP_STANDBYNOW2 = 0x94,
};

enum {
	SG_CDB2_TLEN_NODATA = 0 0,
	SG_CDB2_TLEN_FEAT = 1 0,
	SG_CDB2_TLEN_NSECT = 2 0,
	SG_CDB2_TLEN_BYTES = 0 2,
	SG_CDB2_TLEN_SECTORS = 1 2,
	SG_CDB2_TDIR_TO_DEV = 0 3,
	SG_CDB2_TDIR_FROM_DEV = 1 3,
	SG_CDB2_CHECK_COND = 1 5,
};

static void dump_bytes (const char *prefix, unsigned char *p, int len)
{
	int i;
	if (prefix)
		fprintf(stderr, "%s: ", prefix);
	for (i = 0; i len; i)
		fprintf(stderr, " x", p);
	fprintf(stderr, "\n");
}

#define START_SERIAL 10 /* ASCII serial number */
#define LENGTH_SERIAL 10 /* 10 words (20 bytes or characters) */

void print_ascii(__u16 *p, __u8 length)
{
	__u8 ii;
	char cl;

	/* find first non-space & print it */
	for (ii = 0; ii length; ii ) {
		if(((char) 0x00ff&((*p)>>8)) != ' ') break;
		if((cl = (char) 0x00ff&(*p)) != ' ') {
			if(cl != '\0') printf("%c",cl);
			p ; ii ;
			break;
		}
		p ;
	}
	/* print the rest */
	for (; ii length; ii ) {
		__u8 c;
		/* some older devices have NULLs */
		c = (*p) >> 8;
		if (c) putchar(c);
		c = (*p);
		if (c) putchar(c);
		p ;
	}
	printf("\n");
}

int get_sata_serial(char *szDevice, char *szID)
{
	int fd = 0;
	static __u8 args[512] = { 0 };
	__u16 *id = (void *)(args);

	void *data = (void *)(args);
	unsigned int data_bytes = 512;
	unsigned char cdb[SG_ATA_16_LEN] = { 0 };
	unsigned char sb[32], *desc;
	unsigned char ata_status, ata_error;
	struct sg_io_hdr io_hdr;

	fd = open("/dev/sda", O_RDONLY);
	if (fd 0) {
		printf("open /dev/sda error\n");
		return -1;
	}

	cdb[ 0] = SG_ATA_16;
	cdb[ 1] = SG_ATA_PROTO_PIO_IN;
	cdb[ 2] = SG_CDB2_CHECK_COND;
	cdb[2] |= SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS;
	cdb[2] |= SG_CDB2_TDIR_FROM_DEV;
	cdb[13] = ATA_USING_LBA;
	cdb[14] = ATA_OP_IDENTIFY;
	memset(&(sb[0]), 0, sizeof(sb));
	memset(&io_hdr, 0, sizeof(struct sg_io_hdr));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = SG_ATA_16_LEN;
	io_hdr.mx_sb_len = sizeof(sb);
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = data_bytes;
	io_hdr.dxferp = data;
	io_hdr.cmdp = cdb;
	io_hdr.sbp = sb;
	io_hdr.timeout = 10000; /* msecs */
	if (ioctl(fd, SG_IO, &io_hdr) == -1) {
		fprintf(stderr, "SG_IO ioctl not supported\n");
		return -1; /* SG_IO not supported */
	}
	if (io_hdr.host_status || io_hdr.driver_status != SG_DRIVER_SENSE
			|| (io_hdr.status && io_hdr.status != SG_CHECK_CONDITION))
	{
		errno = EIO;
		return -2;
	}

	if (sb[0] != 0x72 || sb[7] 14) {
		errno = EIO;
		return -3;
	}
	desc = sb 8;
	if (desc[0] != 9 || desc[1] 12){
		errno = EIO;
		return -4;
	}
	ata_error = desc[3];
	ata_status = desc[13];
	if (ata_status & 0x01) { /* ERR_STAT */
		errno = EIO;
		return -5;
	}
	memcpy(szID, &id[START_SERIAL], LENGTH_SERIAL*2);

	print_ascii( &id[START_SERIAL], LENGTH_SERIAL);
	dump_bytes( "Dump Serial:", &id[START_SERIAL], LENGTH_SERIAL*2 );

	return 0;
}

int main()
{
	int rv = 0;
	char szID[64] = { 0 };
	rv= get_sata_serial("/dev/sda", szID);
	if(rv) {
		fprintf(stderr, "get_sata_serial return %d\n", rv);
		return -1;
	}
	printf("Serial: %s\n", szID);
	dump_bytes( "Dump Serial:", szID, sizeof(szID) );
}
