#include <stdio.h>
#include <stdlib.h>
#include <parted/parted.h>

void get_udv_by_name(const char *name)
{
        PedDevice *dev = NULL;
        PedDisk *disk;
        PedPartition *part;

        const char *part_name;
        //static udv_info_t udv_info;
        //udv_info_t *udv = NULL;

        ped_device_probe_all();

        while((dev=ped_device_get_next(dev)))
        {
                printf("dev path: %s, type: %d\n", dev->path, dev->type);

                // 获取所有MD列表
                if (dev->type != PED_DEVICE_MD)
                        continue;

                // 获取当前MD分区信息
                disk = ped_disk_new(dev);
                if (!disk) continue;

                for (part=ped_disk_next_partition(disk, NULL); part;
                                part=ped_disk_next_partition(disk, part))
                {
                        if (part->num < 0) continue;

                        part_name = ped_partition_get_name(part);
                        if ( part_name && !strcmp(part_name, name) )
                        {
                                printf("found %s\n", name);
                                /*
                                udv = &udv_info;
                                strcpy(udv->name, part_name);
                                strcpy(udv->vg_dev, dev->path);
                                udv->part_num = part->num;

                                udv->geom.start = part->geom.start * DFT_SECTOR_SIZE;
                                udv->geom.capacity = part->geom.length * DFT_SECTOR_SIZE;
                                udv->geom.end = udv->geom.start + udv->geom.capacity - 1;
                                */

                                // TODO: udv->state;

                                break;
                        }
                }
                ped_disk_destroy(disk);
        }

        return;
}

int main()
{
        get_udv_by_name("udv1");
}
