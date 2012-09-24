#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

/* from <linux/fs.h> */
#define BLKGETSIZE _IO(0x12,96) /* return device size */
#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */

#define MAX_DISKS 256
#ifdef DEBUG
#define DBG printf
#else
#define DBG
#endif

// 容量的单位
enum UNIT
{
  UNIT_UNDEFINED,
  UNIT_B,
  UNIT_MB,
  UNIT_GB,
  UNIT_TB
};

// 容量结构
struct disk_capacity
{
  uint32_t sector_size;
  uint32_t sectors;
};

int get_disk_capacity(const char *dev, struct disk_capacity *dc);
void usage();

int main(int argc, char *argv[])
{
  enum UNIT unit = UNIT_UNDEFINED;
  int disks, i, j;
  uint64_t disk_size = 0;
  double disk_size_f;
  struct disk_capacity disk_list[MAX_DISKS];

  if (argc<2)
    usage(argv[0]);

  for ( i=1,j=0; i<argc; i++ )
  {
    if ( (UNIT_UNDEFINED==unit) && !strncmp(argv[i], "--unit", 6) )
    {
      if (!argv[i+1])
        usage(argv[0]);

      switch(argv[i+1][0])
      {
        case 'B':
          unit = UNIT_B;
          break;
        case 'M':
          unit = UNIT_MB;
          break;
        case 'G':
          unit = UNIT_GB;
          break;
        case 'T':
          unit = UNIT_TB;
          break;
        default:
          usage();
          break;
      }

      i += 1;
    }
    else if (!get_disk_capacity( argv[i], &disk_list[j] ))
      j++;
  }

  if (UNIT_UNDEFINED==unit)
    unit = UNIT_B;

  DBG("unit = %d\n", unit);
  DBG("i = %d, j = %d\n", i, j);

  for (i=0;i<j;i++)
  {
    DBG("[%.2d] sector_size = %d, sectors = %d\n",
        i, disk_list[i].sector_size, disk_list[i].sectors);
    disk_size += ((uint64_t)disk_list[i].sector_size * (uint64_t)disk_list[i].sectors);
    DBG("disk_size = %lld\n", disk_size);
  }

  switch (unit)
  {
    case UNIT_MB:
      disk_size = (uint64_t)((uint64_t)disk_size/1000/1000);
      break;

    case UNIT_GB:
      disk_size_f = (double)(disk_size/1000.0/1000.0/1000.0);
      break;

    case UNIT_TB:
      disk_size_f = (double)(disk_size/1000.0/1000.0/1000.0/1000.0);
      break;
  }

  if ( (UNIT_B==unit) || (UNIT_MB==unit) )
    fprintf(stdout, "%lld", disk_size);
  else
    fprintf(stdout, "%.2f", disk_size_f);

  return 0;
}

int get_disk_capacity(const char *dev, struct disk_capacity *dc)
{
  int fd;

  DBG("dev=%s\n", dev);

  if ( !dev )
    return -1;

  // 打开设备文件
  if ( (fd=open(dev, O_RDONLY)) <= 0 )
    return -1;

  // 获取磁盘设备的扇区大小
  if ( ioctl(fd, BLKSSZGET, &(dc->sector_size)) )
    return -1;

  DBG("sector_size = %d\n", dc->sector_size);

  // 获取磁盘的扇区数量
  if ( ioctl(fd, BLKGETSIZE, &(dc->sectors)) )
    return -1;

  DBG("sectors = %d\n", dc->sectors);

  close(fd);

  return 0;
}

void usage(const char *prog)
{
  fprintf(stdout, "%s [--unit <B|M|G|T>] <DEVICE> [<DEVICE> ...]\n", prog);
  exit(0);
}

#if 0
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
#endif

