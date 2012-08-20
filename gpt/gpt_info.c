#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define EFI_PART 0x4546492050415254ULL

struct gpt_header
{
	uint64_t signature; // constant value
	uint32_t rev;
	uint32_t header_size; // bytes
	uint32_t crc32;
	uint32_t rsv1;
	uint64_t curr_lba;	// 当前LBA
	uint64_t backup_lba;	// 备份LBA
	uint64_t first_lba;
	uint64_t last_lba;
	uint32_t guid[4];
	uint64_t part_lba;
	uint32_t entries;	// 分区表项数量
	uint32_t entry_size;	// 分区表项大小 （通常128字节）
	uint32_t part_crc32;	// 分区串行CRC32
};

void get_gpt(const char *dev)
{
	int fd;
	struct gpt_header header;

	if ( (fd=open(dev, O_RDONLY)) <= 0)
	{
		perror("open()");
		exit(1);
	}

	lseek(fd, 512, SEEK_SET);
	read(fd, &header, sizeof(struct gpt_header));
	close(fd);

	printf("GPT:\n");
	printf("REVISION: %.8x\n", htonl(header.rev));
	printf("Header size: %u\n", header.header_size);
	printf("Current LBA: %llu\n", header.curr_lba);
	printf("CRC32: %.8p\n", header.crc32);
}

int main(int argc, char *argv[])
{
	get_gpt(argv[1]);
}
