#include <parted/parted.h>
#include <stdio.h>

int main()
{
	PedDevice *dev = NULL;

	ped_device_probe_all();

	while((dev=ped_device_get_next(dev)))
	{
		printf("\n------------------------------\n");
		printf("Name: %s\n", dev->path);
		printf("Model: %s\n", dev->model);
		printf("Capacity: %lld\n", dev->sector_size * dev->length);
		printf("Type: %d\n", dev->type);
	}
}
