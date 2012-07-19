#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>

#define DISK_ID_MAX 64
#define RAID_SLOT_MAX DISK_ID_MAX*UINT8_MAX
#define MAX_DISK 112

/*
ssize_t wis_raid_info(const char *name, int level, int chunk_size;
					  const char *disk_id, int app_type)
{
	
}
*/

const char* const short_options = "n:l:c:d:t:";

const struct option long_options[] = {
	{ "name",         1, NULL, 'm' },
	{ "level",        1, NULL, 'l' },
	{ "chunk_size",   1, NULL, 'c' },
	{ "raid_devices", 1, NULL, 'n' },
	{ "disk_id",      1, NULL, 'd' },
	{ "app_type",     1, NULL, 't' },
	{ 0, 0, 0, 0 }, //最后一个元素标识为NULL
};

void usage()
{
	printf("Usage: Function wis_raid_create \n \
	Optios that are \n \
	--name=         -m  raid name, eg: /dev/md1 or rd1 \n \
	--level=        -l  raid level: 0,1,4,5,6,linear \n \
	--chunk_size=   -c  chunk size of kibibytes \n \
	--raid_devices= -n  chunk size of kibibytes \n \
	--disk_id=      -d  devices to create raid \n \
");
}

int main(int argc, char *argv[])
{
	int c;
	char *_name=NULL, *_disk_id=NULL, *_md_dev=NULL, cmd[1024], line[300];
	int _level, _chunk_size, _app_type, _raid_devices;

	/* check parm */
	if (argc<2)
		usage();
		
	while((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'm':
				_md_dev = optarg;
				printf("the _md_dev1 is %s \n", _md_dev);
				break;
			case 'l':
				if(isdigit(*optarg))
				{
					_level = atoi(optarg);
					printf("the _level is %d \n", _level);
				}
				else
				{
					printf("level must be [0|1|4|5|6] \n");
					//exit(1);
				}
				break;
			case 'c':
				_chunk_size = atoi(optarg);
				printf("the _chunk_size is %d \n", _chunk_size);
				break;
			case 'n':
				_raid_devices = atoi(optarg);
				printf("the _raid_devices is %d \n", _raid_devices);
				break;
			case 'd':
				_disk_id = optarg;
				printf("the _disk_id is %s \n", _disk_id);
				break;
			default:
				usage();
				break;
		}
	}
	
	if(_md_dev != NULL && _chunk_size != 0 && _raid_devices !=0 && _disk_id != NULL)
	{
		/* mdadm -C /dev/md1 -l1 -n2 -c64 /dev/sdc2 /dev/sdc3 */
		sprintf(cmd, "mdadm -CR %s -l%d -c%d -n%d %s 2>&1 \n", _md_dev, _level, _chunk_size, _raid_devices, _disk_id);
		/*printf("the mdadm--create cmd is %s", cmd); */
		FILE *stream = popen(cmd, "r");
		if(!stream)
		{
			printf("mdadm --create raid %s is ERROR \n", _md_dev);
		}
		else
		{
			if(fgets(line, 100, stream) != NULL)
			{
				printf("the result is %s \n", line);
			}
		}
		pclose(stream);
	}
	else
	{
		usage();
	}
		
	return 0;
}

