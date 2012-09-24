#include <stdio.h>
#include <sys/vfs.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int gethd(char *path)
{
  //struct statfs stat;
  //statfs(path,&stat);
  ////printf("total=%dM,free=%dM,%.1f%%\n",stat.f_bsize*stat.f_blocks/1024/1024,stat.f_bsize*stat.f_bfree/1024/1024,(float)stat.f_bfree/(float)stat.f_blocks*100);
  
  struct statvfs stat1;
  statvfs(path,&stat1);
  if(stat1.f_flag)
	printf("%s total=%dK free=%dK %0.1f%%\n",
		path,
		//stat1.f_bsize*stat1.f_blocks/1024,
		stat1.f_bsize*stat1.f_blocks,
		stat1.f_bsize*stat1.f_bfree/1024,
		((float)stat1.f_blocks-(float)stat1.f_bfree)/(float)stat1.f_blocks*100);
}
  

#if 1
int main(int argc, char *argv[])
{
  char disk_dev[128];
  struct statfs buf;
  struct statvfs buf1;
  struct stat buf2;

  if (argc>=2)
	strcpy(disk_dev, argv[1]);
  else
	strcpy(disk_dev, "/dev/dev/sda");

  if (0==statfs(disk_dev, &buf))
  {
	printf("blocks = %ld\n", buf.f_blocks);
  }

  if (0==statvfs(disk_dev, &buf1))
  {
	printf("f_bsize = %ld\n", buf1.f_bsize);
	printf("f_blocks = %ld\n", buf1.f_blocks);
  }

  if (0==stat(disk_dev, &buf2))
  {
	printf("st_size = %lld\n", buf2.st_size);
	printf("ss = %lld\n", (buf2.st_size/1024/1024));
  }
  gethd(disk_dev);

  return 0;
}
#endif

#if 0
int main()
{
  struct stat buf;
  if (0==stat("/dev/dev/sda", &buf))
  {
	printf("st_size = %ld\n", buf.st_size);
  }
  return 0;
}
#endif

