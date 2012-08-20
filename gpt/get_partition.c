#include <error.h>
#include <stdio.h>
#include <parted/parted.h>

int main(int argc, char *argv[])
{
	PedDevice *device;
	PedDisk *disk;
	PedPartition *part;

	if (argc != 2)
		error(EXIT_FAILURE, 0, "wrong number!");

	device = ped_device_get(argv[1]);
	if (!device)
		goto error_out;

	disk = ped_disk_new(device);
	if (!disk)
		goto error;

	printf("%3s %s %s %s\n", "NO.", "Start", "Size", "FS");

	for (part = ped_disk_next_partition(disk, NULL); part;
		part = ped_disk_next_partition(disk, part) )
	{
		if (part->num <  0)
			continue;
		printf("%lld %lld %lld %s\n", part->num,
			part->geom.start, part->geom.length,
			(part->fs_type) ? part->fs_type->name : "");
	}

	ped_disk_destroy(disk);
	ped_device_destroy(device);
	return 0;

error:
	ped_device_destroy(device);
error_out:
	return 1;
}
