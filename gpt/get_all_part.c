#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <parted/parted.h>

//size_t udv_list(udv_info_t *list, size_t n)
int udv_list()
{
        int udv_cnt = 0;

        PedDevice *dev;
        PedDisk *disk;
        PedPartition *part;

        const char *part_name;

        ped_device_probe_all();

        while((dev=ped_device_get_next(dev)))
        {
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
                        if (!part_name) continue;

                        udv_cnt++;
                }
                ped_disk_destroy(disk);
        }

        return udv_cnt;
}

int main()
{
	int cnt = udv_list();
	printf("cnt = %d\n", cnt);
}
