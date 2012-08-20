#include <parted/parted.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	PedDevice *device = NULL;
	PedDiskType *type = NULL;

	if (argc!=2)
		error(EXIT_FAILURE, 0, "wrong arg number!");

	device = ped_device_get(argv[1]);
	if (!device)
		goto error_out;

	type = ped_disk_probe(device);
	if (!type)
		goto error;

	puts("===============");
	puts(argv[1]);
	puts(type->name);

error:
	ped_device_destroy(device);
error_out:
	return 1;
}
