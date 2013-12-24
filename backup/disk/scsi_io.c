#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <errno.h>
#include "scsicmds.h"
#include <stdint.h>

#define STRANGE_BUFFER_LENGTH (4+512*0xf8)
#define HDD_IDENTITY_LEN 20

#define INQUIRY               0x12

typedef enum {
	  // returns no data, just succeeds or fails
	  ENABLE,
	  DISABLE,
	  AUTOSAVE,
	  IMMEDIATE_OFFLINE,
	  STATUS_CHECK, // says if disk's SMART status is healthy, or failing
	  // return 512 bytes of data:
	  READ_VALUES,
	  READ_THRESHOLDS,
	  READ_LOG,
	  IDENTIFY,
	  PIDENTIFY,
	  // returns 1 byte of data
	  CHECK_POWER_MODE,
	  // writes 512 bytes of data:
	  WRITE_LOG
	  } smart_command_set;

// ATA Specification Command Register Values (Commands)
#define ATA_IDENTIFY_DEVICE             0xec                                              
#define ATA_IDENTIFY_PACKET_DEVICE      0xa1
#define ATA_SMART_CMD                   0xb0
#define ATA_CHECK_POWER_MODE            0xe5

/* 
 * 取得scsi设备序列号 
 * 通过SCSI INQUIRY命令取得Vital Product Data(VPD)页面信息 
 * Page Code 80h - Unit serial number  
 */  

int scsi_get_model(const int fd, char *buf)
{
	int i;
	sg_io_hdr_t io_hdr;  
	uint8_t cmd[16] = {0x85, 0x08, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x00};
	uint8_t sense_buff[32], data[512];

	memset( &io_hdr, 0, sizeof( sg_io_hdr_t ) );  
	io_hdr.interface_id = 'S';  

	/* CDB */  
	io_hdr.cmdp = cmd;  
	io_hdr.cmd_len = sizeof( cmd );  

	io_hdr.sbp = sense_buff;
	io_hdr.mx_sb_len = 32;

	io_hdr.dxferp = data;
	io_hdr.dxfer_len = 512;

	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  
	io_hdr.timeout = 2000;

	printf("cmd1: ");
	for(i=0;i<io_hdr.cmd_len;i++)
		printf("%.2x ", cmd[i]);
	puts("");

	if ( ioctl( fd, SG_IO, ( unsigned long )&io_hdr ) < 0 ) {  
		printf("ioctl %s SG_IO failed.\n");  
	}
	
	printf("data1: ");
	for (i=0; i<io_hdr.dxfer_len; i++)
		printf("%.2x ", data[i]);
	puts("\n---------");
	for (i=0; i<io_hdr.dxfer_len; i++) {
		if (isalnum(data[i]))
			printf("%c ", data[i]);
	}
	puts("");
}

int scsi_get_inquiry( const int fd, char *buf )  
{  
	sg_io_hdr_t io_hdr;  

#define VPD_INQUIRY_SIZE  0x00ff   
	uint32_t data_size = VPD_INQUIRY_SIZE;  
	uint8_t data[VPD_INQUIRY_SIZE];  

	uint8_t cmd[] = {  
		INQUIRY,  
		1,  
		0x80,  
		VPD_INQUIRY_SIZE >> 8,  
		VPD_INQUIRY_SIZE & 0xff,  
		0  
	};  

	uint32_t sense_len = 32;  
	uint8_t sense_buffer[sense_len];  

	int len, i, rc = -1;  
	int copy      = 0;  

	cmd[3] = ( data_size>>8 )&0xff;  
	cmd[4] = data_size&0xff;  

	memset( &io_hdr, 0, sizeof( sg_io_hdr_t ) );  
	io_hdr.interface_id = 'S';  

	/* CDB */  
	io_hdr.cmdp = cmd;  
	io_hdr.cmd_len = sizeof( cmd );  

	/* Where to store the sense_data, if there was an error */  
	io_hdr.sbp = sense_buffer;  
	io_hdr.mx_sb_len = sense_len;  
	sense_len = 0;  

	/* Transfer direction, either in or out. Linux does not yet 
		 support bidirectional SCSI transfers ? 
		 */  
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  

	/* Where to store the DATA IN/OUT from the device and how big the 
		 buffer is 
		 */  
	io_hdr.dxferp = data;  
	io_hdr.dxfer_len = data_size;  

	/* SCSI timeout in ms */  
	io_hdr.timeout = 6;  

	printf("cmd: ");
	for(i=0;i<io_hdr.cmd_len;i++)
		printf("%.2x ", cmd[i]);
	puts("");

	if ( ioctl( fd, SG_IO, ( unsigned long )&io_hdr ) < 0 ) {  
		printf("ioctl %s SG_IO failed.\n");  
	}  

	printf("sense buff: ");
	for (i=0; i<io_hdr.mx_sb_len; i++)
		printf("%.2x ", sense_buffer[i]);
	puts("");

	printf("data: ");
	for (i=0; i<io_hdr.dxfer_len; i++)
		printf("%.2x ", data[i]);
	for (i=0; i<io_hdr.dxfer_len; i++)
		printf("%c ", data[i]);
	puts("");

	if ( ( io_hdr.info & SG_INFO_OK_MASK ) != SG_INFO_OK ) {  
		if ( io_hdr.sb_len_wr > 0 ) {  
			sense_len = io_hdr.sb_len_wr;  
			rc = -1;  
			printf ( "INQUIRY sense data: ");  
			for (i = 0; i < io_hdr.sb_len_wr; ++i) {  
				if ((i > 0) && (0 == (i % 10)))  
					printf("\n");  
				printf("0x%02x ", sense_buffer[i] & 0xff);  
			}  
			printf("\n");  
		}  
	}  

	if ( io_hdr.masked_status ) {  
		printf("status=0x%x masked_status=0x%x\n",   
				io_hdr.status, io_hdr.masked_status );  
	}  

	if ( io_hdr.host_status ) {  
		printf("host_status=0x%x\n", io_hdr.host_status );  
	}  

	if ( io_hdr.driver_status ) {  
		printf("driver_status=0x%x\n", io_hdr.driver_status );  
	}  

	/* Page Length */  
	len = data[3];  

	printf("Unit Serial Number Length: %d\n", len );  

	if ( buf ) {  
		if ( len >  HDD_IDENTITY_LEN ) {  
			printf("identity buffer length %d is greater than %d\n",   
					len,   
					HDD_IDENTITY_LEN );  
			copy = 0;  
		}  
		else {  
			copy = 1;  
		}  
		memset( buf, 0x0, HDD_IDENTITY_LEN );  
	}  
#if 1
	/* Unit Serial Number */  
	printf("Unit Serial Number:");   
	for ( i = 4; i < ( len+4 ); i ++ ) {  
		printf( "%c",data[i]&0xff );   
		if ( copy && buf ) {  
			*buf = data[i]&0xff;  
			buf ++;  
		}  
	}  
	printf( "\n" );   
#else   
	if ( copy && buf ) {  
		memcpy( buf, data + 4, len );  
	}  
#endif     
	return 0;
}

/**
 * 去掉字符串中的空格
 * 参数：
 *     str        - 输入字符串
 *     len        - 输入字符串长度
 *     trimed_str - 去掉空格后的字符串
 * 返回值：
 *	0  - 成功，字符串为空
 *	<0 - 失败
 *	>0 - 实际字符串长度
 */
ssize_t trim_space(const char *str, const int len, char *trimed_str)
{
	int i, j;

	for ( i=0,j=0; i<len; i++ )
	{
		if ( 0x20 == str[i] )
			continue;
		trimed_str[j++] = str[i];
	}
	trimed_str[j] = '\0';

	return j;
}

/* 
 * 取得scsi设备序列号 
 * 通过SCSI INQUIRY命令取得Vital Product Data(VPD)页面信息 
 * Page Code 80h - Unit serial number  
 */  
ssize_t get_serialno(const int fd, char *serialno)  
{  
	sg_io_hdr_t io_hdr;  

#define VPD_INQUIRY_SIZE  0x00ff   
	uint32_t data_size = VPD_INQUIRY_SIZE;  
	uint8_t data[VPD_INQUIRY_SIZE];  
	uint8_t buff[HDD_IDENTITY_LEN];  

	uint8_t cmd[] = {  
		INQUIRY,  
		1,  
		0x80,  
		VPD_INQUIRY_SIZE >> 8,  
		VPD_INQUIRY_SIZE & 0xff,  
		0  
	};  

	uint32_t sense_len = 32;  
	uint8_t sense_buffer[sense_len];  

	int len, i, rc = -1;  

	cmd[3] = ( data_size>>8 )&0xff;  
	cmd[4] = data_size&0xff;  

	memset( &io_hdr, 0, sizeof( sg_io_hdr_t ) );  
	io_hdr.interface_id = 'S';  

	/* CDB */  
	io_hdr.cmdp = cmd;  
	io_hdr.cmd_len = sizeof( cmd );  

	/* Where to store the sense_data, if there was an error */  
	io_hdr.sbp = sense_buffer;  
	io_hdr.mx_sb_len = sense_len;  
	sense_len = 0;  

	/* Transfer direction, either in or out. Linux does not yet 
	   support bidirectional SCSI transfers ? 
	   */  
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  

	/* Where to store the DATA IN/OUT from the device and how big the 
	   buffer is 
	   */  
	io_hdr.dxferp = data;  
	io_hdr.dxfer_len = data_size;  

	/* SCSI timeout in ms */  
	io_hdr.timeout = 6;  

	if ( ioctl( fd, SG_IO, ( unsigned long )&io_hdr ) < 0 )
		return -1;

	/* Page Length */  
	len = data[3];  

	if (len > HDD_IDENTITY_LEN)
		return -1;
	memcpy( buff, data + 4, len );  

	return trim_space(buff, len, serialno);
}

#if 0
int ata_getid(int device, char *data)
{
	int copydata;
	unsigned char buff[STRANGE_BUFFER_LENGTH];
	const int HDIO_DRIVE_CMD_OFFSET = 4;
	unsigned short deviceid[256];
	int command = IDENTIFY;

	memset(buff, 0, STRANGE_BUFFER_LENGTH);

	buff[0] = ATA_IDENTIFY_DEVICE;
	buff[3] = 1;
	copydata = 512;

	if (!ioctl(device, HDIO_GET_IDENTITY, deviceid) && (deviceid[0] & 0x8000))
		buff[0]=(command==IDENTIFY)?ATA_IDENTIFY_PACKET_DEVICE:ATA_IDENTIFY_DEVICE;

	// We are now doing the HDIO_DRIVE_CMD type ioctl.
	if ((ioctl(device, HDIO_DRIVE_CMD, buff)))
		return -1;
	if (copydata)
		memcpy(data, buff+HDIO_DRIVE_CMD_OFFSET, copydata);
}
#endif

void get_inquiry(char *dev)
{
	int fd;
	char buff[2048];

	fd = open(dev, O_RDONLY);
	if (fd<0) {
		printf("open %s error!\n", dev);
		return;
	}

	scsi_get_inquiry(fd, buff);
	scsi_get_model(fd, buff);
	close(fd);
}

void scsi_version(const char *dev)
{
  int fd, vers;
  struct sg_scsi_id sgid;
  int serial_len, i;

  UINT8 buf[65535];
  char manufacture[9];
  char product[41];
  char revision[5];
  char data[1024];
  
  if ((fd=open(dev, O_RDONLY))<0)
  {
    perror("open");
    return;
  }

  if (!ioctl(fd, SG_GET_VERSION_NUM, &vers))
  {
    printf("sg dev version: %d\n", vers);
  }

  if (!stdinquiry(fd, (UINT8*)buf))
  {
    memset(manufacture, 0, 9);
    strncpy((char*)&manufacture, (char*)&buf[8], 8);

    memset(product, 0, 41);
    //strncpy((char*)&product, (char*)&buf[16], 16);
    strncpy((char*)&product, (char*)&buf[16], 20);

    memset(revision, 0, 5);
    strncpy((char*)&revision, (char*)&buf[32], 4);

    printf("manufacture: %s\n", manufacture);
    printf("product: %s\n", product);
    printf("revision: %s\n", revision);
  }

  /*
  if (!ata_getid(fd, data))
  {
	  printf("data: %s\n", data);
  }
  */

  /*
  if (!inquiry(fd, 0x80, buf))
  {
	  printf("get 0x80 ok!\n");
  }
  */

  //scsi_get_inquiry(fd, buf);
  serial_len = get_serialno(fd, buf);

  printf("len = %d\n", serial_len);
  printf("serialno: %s\n", buf);

  for (i=0;i<serial_len;i++)
	  printf("%c", buf[i]);
  puts("\n");

  close(fd);
}

int main(const int argc, const char *argv[])
{
	if (argc<2)
		return -1;
	//scsi_version(argv[1]);
	get_inquiry(argv[1]);

	return 0;
}

