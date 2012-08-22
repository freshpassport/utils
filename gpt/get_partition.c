#include <error.h>
#include <stdio.h>
#include <parted/parted.h>

int main(int argc, char *argv[])
{
	PedDevice *device;
	PedDisk *disk;
	PedPartition *part;
	char *name;

	if (argc != 2)
		error(EXIT_FAILURE, 0, "wrong number!");

	device = ped_device_get(argv[1]);
	if (!device)
		goto error_out;

	disk = ped_disk_new(device);
	if (!disk)
		goto error;

	printf("%3s %s %s %s %s\n", "NO.", "Start", "Size", "FS", "NAME");

	for (part = ped_disk_next_partition(disk, NULL); part;
		part = ped_disk_next_partition(disk, part) )
	{
		if (part->num <  0)
			continue;
		name = ped_partition_get_name(part);

		printf("%lld %lld %lld %s %s\n", part->num,
			part->geom.start, part->geom.length,
			(part->fs_type) ? part->fs_type->name : "",
			name ? name : "");
	}

	ped_disk_destroy(disk);
	ped_device_destroy(device);
	return 0;

error:
	ped_device_destroy(device);
error_out:
	return 1;
}
