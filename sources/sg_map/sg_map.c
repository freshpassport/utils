#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <scsi/sg.h>
#include <linux/types.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
typedef struct sg_io_hdr
{
	int interface_id;               /* [i] 'S' (required) */
	int dxfer_direction;            /* [i] */
	unsigned char cmd_len;          /* [i] */
	unsigned char mx_sb_len;        /* [i] */
	unsigned short iovec_count;     /* [i] */
	unsigned int dxfer_len;         /* [i] */
	void * dxferp;                  /* [i], [*io] */
	unsigned char * cmdp;           /* [i], [*i]  */
	unsigned char * sbp;            /* [i], [*o]  */
	unsigned int timeout;           /* [i] unit: millisecs */
	unsigned int flags;             /* [i] */
	int pack_id;                    /* [i->o] */
	void * usr_ptr;                 /* [i->o] */
	unsigned char status;           /* [o] */
	unsigned char masked_status;    /* [o] */
	unsigned char msg_status;       /* [o] */
	unsigned char sb_len_wr;        /* [o] */
	unsigned short host_status;     /* [o] */
	unsigned short driver_status;   /* [o] */
	int resid;                      /* [o] */
	unsigned int duration;          /* [o] */
	unsigned int info;              /* [o] */
} sg_io_hdr_t;  /* 64 bytes long (on i386) */
#endif
 

struct  sg_io_hdr * init_io_hdr()
{
	struct sg_io_hdr * p_scsi_hdr = (struct sg_io_hdr *)malloc(sizeof(struct sg_io_hdr));
	memset(p_scsi_hdr, 0, sizeof(struct sg_io_hdr));
	if (p_scsi_hdr) {
		p_scsi_hdr->interface_id = 'S'; /* this is the only choice we have! */
		/* this would put the LUN to 2nd byte of cdb*/
		p_scsi_hdr->flags = SG_FLAG_LUN_INHIBIT; 
	}
	return p_scsi_hdr;
}

void destroy_io_hdr(struct sg_io_hdr * p_hdr)
{
	if (p_hdr) {
		free(p_hdr);
	}
}

void set_xfer_data(struct sg_io_hdr * p_hdr, void * data, unsigned int length)
{
	if (p_hdr) {
		bzero(data, length);
		p_hdr->dxferp = data;
		p_hdr->dxfer_len = length;
	}
}

void set_sense_data(struct sg_io_hdr * p_hdr, unsigned char * data,
		        unsigned int length)
{
	if (p_hdr) {
		p_hdr->sbp = data;
		p_hdr->mx_sb_len = length;
	}
}
 

int execute_Inquiry(int fd, int page_code, int evpd, struct sg_io_hdr * p_hdr)
{
	unsigned char cdb[6];
	/* set the cdb format */
	cdb[0] = 0x12; /*This is for Inquery*/
	cdb[1] = evpd & 1;
	cdb[2] = page_code & 0xff;
	cdb[3] = 0;
	cdb[4] = 0xff;
	cdb[5] = 0; /*For control filed, just use 0 */

	p_hdr->dxfer_direction = SG_DXFER_FROM_DEV;
	p_hdr->cmdp = cdb;
	p_hdr->cmd_len = 6;

	int ret = ioctl(fd, SG_IO, p_hdr);
	if (ret<0) {
		printf("Sending SCSI Command failed.\n");
		close(fd);
		exit(1);
	}
	return p_hdr->status;
}

void show_sense_buffer(struct sg_io_hdr *p_hdr)
{
	int i;
	
	printf("SENSE BUF(%d): ", p_hdr->mx_sb_len);
	for (i=0; i<p_hdr->mx_sb_len; i++)
		printf("%.2x ", p_hdr->sbp[i]);
	printf("\n");
}
 
void show_xfer_data(struct sg_io_hdr *p_hdr)
{
	int i;
	unsigned char *data = p_hdr->dxferp;

	printf("XFER DATA(%d): ", p_hdr->dxfer_len);
	for (i=0; i<p_hdr->dxfer_len; i++)
		printf("%c ", data[i]);
	printf("\n");
}

void show_vendor(struct sg_io_hdr * hdr)
{
	unsigned char * buffer = hdr->dxferp;
	int i;
	printf("vendor id:");
	for (i=8; i<16; ++i) {
		putchar(buffer[i]);
	}
	putchar('\n');
}

void show_product(struct sg_io_hdr * hdr)
{
	unsigned char * buffer = hdr->dxferp;
	int i;
	printf("product id:");
	for (i=16; i<32; ++i) {
		putchar(buffer[i]);
	}
	putchar('\n');
}

void show_product_rev(struct sg_io_hdr * hdr)
{
	unsigned char * buffer = hdr->dxferp;
	int i;
	printf("product ver:");
	for (i=32; i<36; ++i) {
		putchar(buffer[i]);
	}
	putchar('\n');
}

#if 0
void show_serial_number(struct sg_io_hdr *hdr)
{
	unsigned char *buffer = hdr->dxferp;
	int i;
	printf("serial number: ");
	for (i=0x80, 
}
#endif

#define SENSE_LEN 32
#define BLOCK_LEN 1

unsigned char sense_buffer[SENSE_LEN];
unsigned char data_buffer[BLOCK_LEN*256];
void test_execute_Inquiry(char * path, int evpd, int page_code)
{
	struct sg_io_hdr * p_hdr = init_io_hdr();
	set_xfer_data(p_hdr, data_buffer, BLOCK_LEN*256);
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	int status = 0;
	int fd = open(path, O_RDWR);
	if (fd>0) {
		status = execute_Inquiry(fd, page_code, evpd, p_hdr);
		printf("the return status is %d\n", status);
		if (status!=0) {
			show_sense_buffer(p_hdr);
		} else{
			show_vendor(p_hdr);
			show_product(p_hdr);
			show_product_rev(p_hdr);
			show_xfer_data(p_hdr);
		}
	} else {
		printf("failed to open sg file %s\n", path);
	}
	close(fd);
	destroy_io_hdr(p_hdr);
}


void test_cmds(char *path, int evpd, int page_code)
{
	struct sg_io_hdr * p_hdr = init_io_hdr();
	set_xfer_data(p_hdr, data_buffer, BLOCK_LEN*256);
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	int status = 0;
	int fd = open(path, O_RDWR);
	if (fd>0) {
		status = execute_Inquiry(fd, page_code, evpd, p_hdr);
		printf("the return status is %d\n", status);
		//show_sense_buffer(p_hdr);
		show_xfer_data(p_hdr);
	} else {
		printf("failed to open sg file %s\n", path);
	}
	close(fd);
	destroy_io_hdr(p_hdr);
}

void get_rpm(char *path, int evpd, int page_code)
{
	struct sg_io_hdr * p_hdr = init_io_hdr();
	set_xfer_data(p_hdr, data_buffer, BLOCK_LEN*256);
	set_sense_data(p_hdr, sense_buffer, SENSE_LEN);
	int status = 0;
	unsigned char *xfer;
	int pdt, rpm;

	int fd = open(path, O_RDWR);
	if (fd>0) {
		status = execute_Inquiry(fd, page_code, evpd, p_hdr);
		printf("the return status is %d\n", status);
		//show_sense_buffer(p_hdr);
		//show_xfer_data(p_hdr);

		// 参考代码 sg3_utils: sg_vpd.c : decode_b1_vpd()
		xfer = p_hdr->dxferp;
		pdt = (int)xfer[0];
		switch (pdt)
		{
			case 0:
				if (p_hdr->dxfer_len < 64)
				{
					printf("xfer is too short!\n");
				}
				else
				{
					rpm = (xfer[4]<<8) | xfer[5];
					printf("rpm of %s: %d rpm\n", path, rpm);
				}
				break;
			default:
				printf("unknown pdt\n");
				break;
		}
	} else {
		printf("failed to open sg file %s\n", path);
	}
	close(fd);
	destroy_io_hdr(p_hdr);
}

int main(int argc, char * argv[])
{
	//test_execute_Inquiry(argv[1], 0, 0);

	//test_cmds(argv[1], 1, 0x86);
	get_rpm(argv[1], 1, 0xb1); // rpm
	// tested:
	// 0x80,0x83

	return 0;
}
