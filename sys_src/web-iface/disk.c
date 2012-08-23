#include <unistd.h>
#include <getopt.h>

#include <wis/disk.h>
#include <wis/size-unit-conv.h>
#include <wis/common/debug.h>

#include "web-iface.h"
#include "common.h"

struct option disk_options[] = {
	{"list",		no_argument,		NULL,	'l'},
	{"get_disk_info",	required_argument,	NULL,	'i'},
	{"get_disk_detail",	required_argument,	NULL,	'd'},
	{"set_special_spare",	required_argument,	NULL,	's'},
	{"set_global_spare",	required_argument,	NULL,	'g'},
	{"set_free",		required_argument,	NULL,	'f'},
	{"list_smartattr",	required_argument,	NULL,	'm'},
	{"locate",		required_argument,	NULL,	'L'},
	{"locate_state",	no_argument,		NULL,	'S'},
	{"locate_off",		no_argument,		NULL,	'O'},
	{"get_stat",		no_argument,		NULL,	'T'},
	{"get_global_capacity",	no_argument,		NULL,	'C'},
	{"get_online_list",	no_argument,		NULL,	'I'},
	{"get_alarm_list",	no_argument,		NULL,	'A'},
	{0, 0, 0, 0}

};

void disk_usage()
{
  printf(_T("\nwis_disk\n\n"));
  printf(_T("Usage: --list\n"));
  printf(_T("       --get_disk_info <disk_id>\n"));
  printf(_T("       --get_disk_detail <disk_id>\n"));
  printf(_T("       --set_special_spare <raid_id> <disk_id[,...]>\n"));
  printf(_T("       --set_global_spare <disk_id[,...]>\n"));
  printf(_T("       --set_free <disk_id[,...]>\n"));
  printf(_T("       --list_smartattr [disk_id[,...]]\n"));
  printf(_T("       --locate <disk_id>\n"));
  printf(_T("       --locate_state\n"));
  printf(_T("       --locate_off\n"));
  printf(_T("       --get_stat\n"));
  printf(_T("       --get_global_capacity\n"));
  printf(_T("       --get_online_list\n"));
  printf(_T("       --get_alarm_list\n"));
  printf(_T("\n\n"));
  exit(0);
}

/* list / get_disk_info接口显示结果 */
void show_disk_info(struct disk_info *dinfo)
{
	struct disk_info *p;

	if (!dinfo)
		return;

	// 输出磁盘列表信息
	p = dinfo;
	printf("enclosure='%d';", diskid_str2enclosure(p->id));
	printf("slot='%d';", diskid_str2slot(p->id));
	printf("disk_dev='%s';", p->dev);
	printf("state='%s';", disk_smart_health2str(p->smart.health));
	//printf("mng_state='%s';",);
	printf("smartinfo='%s';",
			disk_smart_health2str(p->smart.health));
	printf("model_name='%s';", p->model);
	printf("serialno='%s';", p->serial);
	printf("size='%s';", byte2str(p->capacity, UNIT_MAX, D1000));
	printf("firmware='%s';", p->firmware);
	/* TODO: raid support
	   printf("md_dev=");
	   printf("raid_name=");
	   printf("raid_level=");
	   printf("rdcy=");
	   */
	printf("\n");
}

/* get_disk_detail接口显示结果 */
void show_disk_detail(struct disk_info *dinfo, size_t n)
{
}

/* get_smartattr接口显示结果 */
void show_disk_smartattr(struct disk_info *dinfo, size_t n)
{
}

/* 打印磁盘列表到屏幕 */
void disk_list(const char *disk_id)
{
	// 磁盘数量
	ssize_t disk_count = 0;
	// 最大可获取磁盘列表数量
	struct disk_info dinfo[DISK_ID_MAX];
	int i;
	char full_dev[128];

	// 处理获取指定磁盘的参数
	if (disk_id)
	{
		if (is_disk_id(disk_id))
			if (false == slot2fulldev(disk_id, full_dev))
			{
				ERROR("请输入正确的磁盘ID!");
				exit(-1);
			}
	}

	// 获取磁盘列表
	disk_count = wis_disk_info(dinfo, DISK_ID_MAX);

	// 获取磁盘列表出错
	if (disk_count < 0)
	{
		ERROR("无法获取磁盘列表!");
		exit(-1);
	}

	// 当前系统没有磁盘
	if (disk_count == 0)
		exit(0);

	for (i=0; i<disk_count; i++)
	{
		// 显示磁盘列表
		if (!disk_id)
		{
			show_disk_info(&dinfo[i]);
			continue;
		}

		// 显示指定磁盘
		if (disk_id && (!strcmp(dinfo[i].dev, full_dev)) )
		{
			show_disk_info(&dinfo[i]);
			exit(0);
		}
	}

	// 没有找到指定的磁盘
	if (disk_id)
	{
		ERROR("磁盘ID不存在!");
		exit(-1);
	}

	exit(0);
}

void disk_detail(const char *disk_id)
{
}

void global_capacity()
{
	size_t disk_count = 0;
	struct disk_info dinfo[DISK_ID_MAX];
	loff_t capacity = 0;
	int i;

	// 获取磁盘列表
	disk_count = wis_disk_info(dinfo, DISK_ID_MAX);

	// 获取磁盘列表出错
	if (disk_count < 0)
	{
		ERROR("无法获取磁盘列表！");
		exit(-1);
	}

	// 当前系统没有磁盘
	if (disk_count == 0)
		exit(0);

	for (i=0; i<disk_count; i++)
		capacity += dinfo[i].capacity;

	printf("global_capacity='%llu'\n", capacity);

	exit(0);
}

void exec_old_cmd(const char *cmd)
{
	DBGP("orig_cmd: %s\n", cmd);
	exit(system(cmd));
}

int wis_disk_main(int argc, char *argv[])
{
	char c, orig_cmd[256];

	//opterr = 0;  // 关闭错误提示
	while( (c=getopt_long(argc, argv, "", disk_options, NULL)) != -1 )
	{
		DBGP("getopt_long() return: %c\n", c);
		switch (c)
		{
			case 'l':  // --list
				disk_list(NULL);
				break;
			case 'i':  // --get_disk_info
				disk_list(optarg);
				break;
			case 'd':  // --get_disk_detail
				disk_detail(optarg);
				break;
			case 's':  // --set_special_spare
				sprintf(orig_cmd,
				"wis_disk_old --set_special_spare %s %s",
				optarg, optarg+1);
				exec_old_cmd(orig_cmd);
				break;
			case 'g':  // --set_global_spare
				sprintf(orig_cmd,
				"wis_disk_old --set_global_spare %s", optarg);
				exec_old_cmd(orig_cmd);
				break;
			case 'f':  // --set_free
				sprintf(orig_cmd,
					"wis_disk_old --set_free %s", optarg);
				exec_old_cmd(orig_cmd);
				break;
			case 'm':  // --list_smartattr
				break;
			case 'L':  // --locate
				sprintf(orig_cmd,
					"wis_disk_old --locate %s", optarg);
				exec_old_cmd(orig_cmd);
				break;
			case 'S':  // --locate_state
				sprintf(orig_cmd,
					"wis_disk_old --locate_state");
				exec_old_cmd(orig_cmd);
				break;
			case 'O':  // --locate_off
				sprintf(orig_cmd,
					"wis_disk_old --locate_off");
				exec_old_cmd(orig_cmd);
				break;
			case 'T':  // --get_stat
				sprintf(orig_cmd, "wis_disk_old --get_stat");
				exec_old_cmd(orig_cmd);
				break;
			case 'C':  // --get_global_capacity
				global_capacity();
				break;
			case 'I':  // --get_online_list
				sprintf(orig_cmd,
					"wis_disk_old --get_online_list");
				exec_old_cmd(orig_cmd);
				break;
			case 'A':  // --get_alarm_list
				sprintf(orig_cmd,
					"wis_disk_old --get_alarm_list");
				exec_old_cmd(orig_cmd);
				break;
			case '?':
			default:
				disk_usage();
				break;
		}
	}

	disk_usage();

	return 0;
}
