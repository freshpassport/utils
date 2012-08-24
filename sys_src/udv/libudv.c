#include <parted/parted.h>
#include "libudv.h"

const static int DFT_SECTOR_SIZE = 512;

PedDisk*
_create_disk_label (PedDevice *dev, PedDiskType *type)
{
        PedDisk* disk = NULL;

        /* Create the label */
        disk = ped_disk_new_fresh (dev, type);
        /*
         *         fail_if (!disk, "Failed to create a label of type: %s",
         *                          type->name);
         *                                  fail_if (!ped_disk_commit(disk),
         *                                               "Failed to commit label to device");
         *                                                               */
        if (!disk)
                return NULL;
        if (!ped_disk_commit(disk))
                return NULL;

        return disk;
}

/**
 * API
 */

ssize_t udv_create(const char *vg_name, const char *name, uint64_t capacity)
{
        udv_info_t udv_list[MAX_UDV], *udv;
        size_t udv_cnt = 0, i;

        PedDevice *device;
        PedDisk *disk;
        PedPartition *part;
        PedConstraint *constraint;

	const char *vg_dev;

        // 参数检查
        if (!name)
                return EINVAL;

        // 检查用户数据卷是否存在
        if (udv_exist(name))
                return EEXIST;

        // 检查VG是否存在
        if (!(vg_dev=vg_name2dev(vg_name)))
                return EEXIST;


        // 创建用户数据卷
        if (!(device = ped_device_get(vg_dev)))
                goto error_eio;

        if (!(disk = _create_disk_label(device, ped_disk_type_get("gpt"))))
                goto error;

        if (!(constraint = ped_constraint_any(device)))
                goto error;
        
        // 获取当前VG的所有分区
        udv = &udv_list[0];
        for (part=ped_disk_next_partition(disk, NULL); part;
                        part = ped_disk_next_partition(disk, part))
        {
                udv->part_num = part->num;
                udv->geom.start = part->geom.start * DFT_SECTOR_SIZE;
                udv->geom.capacity = part->geom.length * DFT_SECTOR_SIZE;
                udv->geom.end = udv->geom.start + udv->geom.capacity - 1;
                udv++; udv_cnt++;
        }

        // TODO: 查找空隙
        for (i=0; i<udv_cnt; i++)
        {
        }

error:
        ped_device_destroy(device);
error_eio:
        return EIO;
success:
        return 0;
}

/**
 * @breif 删除指定名称的用户数据卷
 * @param name - 被删除用户数据卷名称
 * @return EINVAL - 参数错误
 *         ENODEV - 用户数据卷不存在
 *         EIO - 设置失败
 *         0 - 成功
 */
ssize_t udv_delete(const char *name)
{
        udv_info_t *udv;
        size_t udv_cnt, i;

        PedDevice *device;
        PedDisk *disk;
        PedPartition *part;

        // 参数检查
        if (!name)
                return EINVAL;

        // 查找UDV
        if (!(udv=get_udv_by_name(name)))
                return ENODEV;

        // 删除分区
        if (!(device = ped_device_get(udv->vg_dev)))
                goto error_eio;

        if (!(disk = ped_disk_new(device)))
                goto error;

        if (!(part = ped_disk_get_partition(disk, udv->part_num)))
                goto error;

        if (ped_disk_delete_partition(disk, part) &&
                ped_disk_commit(disk))
                goto success;

error:
        ped_device_destroy(device);
error_eio:
        return EIO;
success:
        return 0;
}

size_t udv_list(udv_info_t *list, size_t n)
{
        size_t udv_cnt = 0;

        PedDevice *dev = NULL;
        PedDisk *disk;
        PedPartition *part;

        const char *part_name;
        udv_info_t *udv = list;

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

                        if (udv_cnt >= n)
                                break;

                        strcpy(udv->name, part_name);
                        strcpy(udv->vg_dev, dev->path);
                        udv->part_num = part->num;

                        udv->geom.start = part->geom.start * DFT_SECTOR_SIZE;
                        udv->geom.capacity = part->geom.length * DFT_SECTOR_SIZE;
                        udv->geom.end = udv->geom.start + udv->geom.capacity - 1;

                        // TODO: udv->state;

                        udv_cnt++; udv++;
                }
                ped_disk_destroy(disk);
        }

        return udv_cnt;
}

udv_info_t* get_udv_by_name(const char *name)
{
        PedDevice *dev = NULL;
        PedDisk *disk;
        PedPartition *part;

        const char *part_name;
        static udv_info_t udv_info;
        udv_info_t *udv = NULL;

        ped_device_probe_all();

        while((dev=ped_device_get_next(dev)))
        {
                // 获取所有MD列表
                if (dev->type != PED_DEVICE_MD)
                        continue;

                // 获取当前MD分区信息
                if ( !(disk=ped_disk_new(dev)) )
			continue;

                for (part=ped_disk_next_partition(disk, NULL); part;
                                part=ped_disk_next_partition(disk, part))
                {
                        if (part->num < 0) continue;

                        part_name = ped_partition_get_name(part);
                        if ( part_name && !strcmp(part_name, name) )
                        {
                                udv = &udv_info;
                                strcpy(udv->name, part_name);
                                strcpy(udv->vg_dev, dev->path);
                                udv->part_num = part->num;

                                udv->geom.start = part->geom.start * DFT_SECTOR_SIZE;
                                udv->geom.capacity = part->geom.length * DFT_SECTOR_SIZE;
                                udv->geom.end = udv->geom.start + udv->geom.capacity - 1;

                                // TODO: udv->state;
				udv->state = UDV_RAW; // for debug

				ped_disk_destroy(disk);
				return udv;

                        }
                }
                ped_disk_destroy(disk);
        }

        return NULL;
}

/**
 * @param name - 被修改用户数据卷名称 
 * @param new_name - 用户数据卷新名称
 * @return
 *      EINVAL - 参数错误
 *      ENODEV - 用户数据卷不存在
 *      EEXIST - 用户数据卷新名称存在
 *      EIO    - 设置失败
 *      0 - 成功
 */
ssize_t udv_rename(const char *name, const char *new_name)
{
        udv_info_t list[MAX_UDV], *udv;
        size_t udv_cnt, i;
        bool old_exist = false, new_exist = false;

        PedDevice *device;
        PedDisk *disk;
        PedPartition *part;

        // 参数检查
        if (!(name && new_name))
                return EINVAL;

        // 查找UDV
        if ((udv_cnt=udv_list(list, MAX_UDV))==0)
                return ENODEV;

        udv = &list[0];
        for (i=0; i<udv_cnt; i++)
        {
                if (!strcmp(name, udv->name))
                        old_exist = true;
                if (!strcmp(new_name, udv->name))
                        new_exist = true;
        }

        // 检查当前UDV是否存在
        if (!old_exist)
                return ENODEV;

        // 检查新的UDV是否存在
        if (new_exist)
                return EEXIST;

        // 修改名称
        if (!(device = ped_device_get(udv->vg_dev)))
                goto error_eio;

        if (!(disk = ped_disk_new(device)))
                goto error;

        if (!(part = ped_disk_get_partition(disk, udv->part_num)))
                goto error;

        if (ped_partition_set_name(part, new_name) &&
                ped_disk_commit(disk))
                goto success;

error:
        ped_device_destroy(device);
error_eio:
        return EIO;
success:
        return 0;
}

/**
 * Utils
 */


/**
 * @param name - 用户数据卷名称
 */
bool udv_exist(const char *name)
{
        udv_info_t list[MAX_UDV];
        size_t udv_cnt, i;

        // 参数检查
        if (!name)
                return EINVAL;

        // 查找UDV
        if ((udv_cnt=udv_list(list, MAX_UDV))==0)
                return ENODEV;

        for (i=0; i<udv_cnt; i++)
                if (!strcmp(name, list[i].name))
                        return true;
        return false;
}

const char* vg_name2dev(const char *name)
{
        return NULL;
}

const char* vg_dev2name(const char *dev)
{
        return NULL;
}

const char* udv_name2dev(const char *name)
{
        return NULL;
}

const char* dev_dev2name(const char *dev)
{
        return NULL;
}
