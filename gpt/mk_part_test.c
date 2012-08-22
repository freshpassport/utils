#include <stdio.h>
#include <error.h>
#include <parted/parted.h>

PedDisk*
_create_disk_label (PedDevice *dev, PedDiskType *type)
{
        PedDisk* disk = NULL;

        /* Create the label */
        disk = ped_disk_new_fresh (dev, type);
	/*
        fail_if (!disk, "Failed to create a label of type: %s",
                 type->name);
        fail_if (!ped_disk_commit(disk),
		 "Failed to commit label to device");
		 */
	if (!disk)
		return NULL;
	if (!ped_disk_commit(disk))
		return NULL;

        return disk;
}

int main(int argc, char *argv[])
{
	PedDevice *dev;
	PedDisk *disk;
	PedPartition *part;
	PedConstraint *constraint;

	if (argc!=2)
		exit(1);

	dev = ped_device_get(argv[1]);
	if (!dev)
	{
		perror("get_device");
		return -1;
	}

	disk = _create_disk_label(dev, ped_disk_type_get("gpt"));
	if (!disk)
	{
		fprintf(stderr, "fail to create disk label gpt\n");
		return -2;
	}
	constraint = ped_constraint_any(dev);

	// part1: 17.4Kb ~ 15MB
	part = ped_partition_new(disk, PED_PARTITION_NORMAL,
				NULL,
				34, 29296);
	ped_disk_add_partition(disk, part, constraint);

	// part2: 15MB ~ 35MB
	part = ped_partition_new(disk, PED_PARTITION_NORMAL,
				NULL,
				29297, 71680);
	ped_partition_set_name(part, "udv2");
	ped_disk_add_partition(disk, part, constraint);

	ped_disk_commit(disk);

	ped_constraint_destroy(constraint);
	ped_disk_destroy(disk);
	ped_device_destroy(dev);

	return 0;
}
