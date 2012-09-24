#include <sys/vfs.h>
#include <stdio.h>

int main()
{
	struct statfs diskInfo;
	statfs("/dev/sda",&diskInfo);
	unsigned long long totalBlocks = diskInfo.f_bsize;
	unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;
	printf("total blocks: %ld\n", diskInfo.f_blocks);
	printf("block size: %d\n", diskInfo.f_bsize);
	printf("TOTAL_SIZE == %lld MB\n",totalSize>>20);
	unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;
	printf("DISK_FREE == %lld MB\n",freeDisk>>20);
	//return freeDisk>>20;
}

