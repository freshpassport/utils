#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* from <linux/fs.h> */
#define BLKGETSIZE _IO(0x12,96) /* return device size */
#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */

int main()
{
  int fd;
  unsigned int sector_size = 0,
      disk_sectors = 0;
  unsigned long long disk_size;

  if ( (fd = open("/dev/sda", O_RDONLY)) <=0 )
  {
    return 0;
  }

  if ( ! ioctl(fd, BLKSSZGET, &sector_size) )
  {
    printf("disk sector size: %d\n", sector_size);
  }

  if ( ! ioctl(fd, BLKGETSIZE, &disk_sectors) )
  {
    printf("disk sectors: %d\n", disk_sectors);
  }

  if ( sector_size && disk_sectors )
  {
    disk_size = (unsigned long long)sector_size * (unsigned long long)disk_sectors;
    printf("disk size: %lld B\n", disk_size);
    printf("           %lld MB\n", disk_size/1000/1000);
  }

  close(fd);

  return 0;
}

