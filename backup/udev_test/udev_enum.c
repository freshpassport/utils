#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

int main()
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry, *sysattrs, *sysattr_list_entry;
	struct udev_device *dev;

	udev = udev_new();
	if (!udev)
	{
		printf("Can't create udev!\n");
		exit(1);
	}

	/* Create a list of the device in 'hidraw' subsystem */
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "block");
	//udev_enumerate_add_match_subsystem(enumerate, "usb");
	//udev_enumerate_add_match_property(enumerate, "DEVTYPE", "disk");
	//udev_enumerate_add_match_property(enumerate, "ID_BUS", "scsi");
	//udev_enumerate_add_nomatch_sysattr(enumerate, "partition", "");
	udev_enumerate_scan_devices(enumerate);

	devices = udev_enumerate_get_list_entry(enumerate);

	udev_list_entry_foreach(dev_list_entry, devices)
	{
		const char *path;
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);
		
		printf("Device Node Path: %s, DEV_NUM: %x, SYS: %s\n",
				udev_device_get_devnode(dev),
				udev_device_get_devnum(dev),
				udev_device_get_syspath(dev));

#if 0
		printf("--------attr----------\n");
		//sysattrs = udev_device_get_devlinks_list_entry(dev);
		//sysattrs = udev_device_get_tags_list_entry(dev);
		//sysattrs = udev_device_get_properties_list_entry(dev);
		sysattrs = udev_device_get_sysattr_list_entry(dev);
		udev_list_entry_foreach(sysattr_list_entry, sysattrs)
		{
			printf("ATTR: %s, VALUE: %s\n",
					udev_list_entry_get_name(sysattr_list_entry),
					udev_list_entry_get_value(sysattr_list_entry));
		}
#endif
	}

	udev_unref(udev);
	return 0;
}

