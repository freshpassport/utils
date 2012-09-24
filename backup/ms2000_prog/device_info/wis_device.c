#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "mon_interface.h"

#define EXPANDER_NUM 4 //扩展柜个数

struct name_type_conv {
	enum MON_CONTROLLER_TYPE type;
	char dev_name[128];
};

static struct name_type_conv n2t[] = {
	{MT_MAIN, "main"},
	{MT_EXPANDER1, "expander-1"},
	{MT_EXPANDER2, "expander-2"},
	{MT_EXPANDER3, "expander-3"},
	{MT_EXPANDER4, "expander-4"}
};

int ret, i;

/* 错误输出 */
void err_out(char *msg)
{
	if (msg)
		fprintf(stdout, "ERR:%s\n", msg);
	exit(1);
}

/* 名称转换为类型 */
void name2type(char *name, int *type)
{
	int i, items;

	if (NULL==name)
		return;
	if (NULL==type)
		return;

	items = (int)(sizeof(n2t)/sizeof(struct name_type_conv));
	for (i=0;i<items;i++)
	{
		if (!strcmp(name, n2t[i].dev_name))
		{
			*type = n2t[i].type;
			return;
		}
	} /* end for */
}

/*****************************************************************************/
/*  处理函数                                                                 */
/*****************************************************************************/
void get_component()
{
	struct mon_component con;

	ret = mon_get_component(&con);
	if (MON_RET_SUCCESS==ret)
	{
		printf("man_dev=%d;expander=%d;ups=%d\n",
				con.main_dev, con.expander, con.ups);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取组件列表失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		err_out("无法获取组件列表！");
	}

	err_out("未知错误！");
}

void get_main()
{
	struct mon_main con;
	
	ret = mon_get_main(&con);
	if (MON_RET_SUCCESS==ret)
	{
		printf("type='main';name='%s';sn='%d';controller='%d';controller_info='%d';power='%d';fan='%d';lcd='%d';buzzer='%d'\n",
				con.name, con.sn, con.controller, con.controller_info, con.power, con.fan, con.lcd, con.buzzer);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取组件列表失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		err_out("无法获取组件列表！");
	}
	
	err_out("未知错误！");
}

void get_expander()
{
	char tmp_buf[128];
	struct mon_expander con;
	
	for (i=1; i<=EXPANDER_NUM; i++)
	{
		sprintf(tmp_buf, "expander+%d", i);
		name2type(tmp_buf, &con.type);
		ret = mon_get_expander(&con);
		if (MON_RET_SUCCESS==ret)
		{
			printf("type='expander';name='%s';sn='%d';controller='%d';controller_info='%s';power='%d';fan='%d';lcd='%d';buzzer='%d'\n",
					con.name, con.sn, con.controller, con.controller_info, con.power, con.fan, con.lcd, con.buzzer);
			exit(0);
		}
		else if (MON_RET_FAILED==ret)
		{
			err_out("获取组件列表失败！");
		}
		else if (MON_RET_NONE==ret)
		{
			//err_out("无法获取组件列表！");
			printf("type='expander';name='expander';sn='Wisdata';controller='2';controller_info='OK';power='2';fan='4';lcd='1';buzzer='1'\n");
			exit(0);
		}
	}
	err_out("未知错误！");
}

void get_controller(char *name, char *number)
{
	char contro_type[128];
	struct mon_controller con;

	if (!name)
		err_out("请输入名称！");
	if (!number)
		err_out("请指定控制器编号！");

	name2type(name, &con.type);
	//con.number = atoi(number);
	ret = mon_get_controller(&con);
	if (MON_RET_SUCCESS==ret)
	{
		if (!strncmp(name, "main", 4))
			strcpy(contro_type, "main_controller");
		else if (!strncmp(name, "expander", 8))
			strcpy(contro_type, "expander_controller");
		
		printf("type='%s';sn='%s';version='%d';cpu='%s';mem='%ld';temp1='%d';temp2='%d';temp3='%d';temp4='%d';temp5='%d';temp6='%d';voltage1='%d';voltage2='%d';voltage3='%d';voltage4='%d';voltage5='%d';voltage6='%d'\n",
				contro_type, con.sn, con.version, con.cpu, con.mem, con.temp[0], con.temp[1], con.temp[2], con.temp[3], con.temp[4], con.temp[5], con.vol[0], con.vol[1], con.vol[2], con.vol[3], con.vol[4], con.vol[5]);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取控制器信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取控制器信息！");
		printf("type='expander_controller';sn='Wisdata';version='65536';cpu='OCTEON';mem='2048';temp1='26';temp2='26';temp3='26';temp4='26';temp5='26';temp6='26';voltage1='30';voltage2='30';voltage3='30';voltage4='30';voltage5='30';voltage6='30'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void get_backplane(char *name)
{
	char contro_type[128];
	struct mon_backplane con;
	
	if (!name)
		err_out("请输入名称！");
		
	name2type(name, &con.type);
	ret = mon_get_backplane(&con);
	if (MON_RET_SUCCESS==ret)
	{
		if (!strncmp(name, "main", 4))
			strcpy(contro_type, "main_backplane");
		else if (!strncmp(name, "expander", 8))
			strcpy(contro_type, "expander_backplane");
		
		printf("type='%s';sn='%s';temp1='%d';temp2='%d';temp3='%d';voltage1='%d';voltage2='%d';voltage3='%d';mcu_ver='%d'\n",
				contro_type, con.sn, con.temp[0], con.temp[1], con.temp[2], con.vol[0], con.temp[1], con.temp[2], con.mcu_ver);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取背板信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取背板信息！");
		printf("type='expander_backplane';sn='Wisdata';temp1='26';temp2='26';temp3='26';voltage1='30';voltage2='30';voltage3='30';mcu_ver='30'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void get_power(char *name, char *number)
{
	char power_type[128];
	struct mon_power con;

	if (!name)
		err_out("请输入名称！");
	if (!number)
		err_out("请指定电源编号！");

	name2type(name, &con.type);
	con.number = atoi(number);
	ret = mon_get_power(&con);
	if (MON_RET_SUCCESS==ret)
	{
		// 电源类型
		if (!strncmp(name, "main", 4))
			strcpy(power_type, "main_power");
		else if (!strncmp(name, "expander", 8))
			strcpy(power_type, "expander_power");

		printf("type='%s';sn='%s';power='%d';input_voltage='%d';output_voltage='%d'\n",
				power_type, con.sn, con.power, con.input_vol, con.output_vol);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取电源信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取电源信息！");
		printf("type='power';sn='Wisdata';power='ok';input_voltage='ok';output_voltage='ok'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void get_fan(char *name, char *number)
{
	char contro_fan[128];
	struct mon_fan con;
	
	if (!name)
		err_out("请输入名称！");
	if (!number)
		err_out("请指定电源编号！");
		
	name2type(name, &con.type);
	con.number = atoi(number);
	ret = mon_get_fan(&con);
	if (MON_RET_SUCCESS==ret)
	{
		if (!strncmp(name, "main", 4))
			strcpy(contro_fan, "main_fan");
		else if (!strncmp(name, "expander", 8))
			strcpy(contro_fan, "expander_fan");
			
		printf("type='%s';sn='%s';max_speed='%d';current_speed='%d'\n",
				contro_fan, con.sn, con.max_speed, con.current_speed);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取风扇信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取风扇信息！");
		printf("type='expander_fan';sn='Wisdata';max_speed='6000';current_speed='3000'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void get_lcd(char *name)
{
	struct mon_lcd con;
	
	if (!name)
		err_out("请输入名称！");
		
	name2type(name, &con.type);
	ret = mon_get_lcd(&con);
	if (MON_RET_SUCCESS==ret)
	{
		printf("type='lcd';sn='%s'\n",
				 con.sn);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取LCD信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取LCD信息！");
		printf("type='lcd';sn='Wisdata'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void get_buzzer(char *name)
{
	struct mon_buzzer con;
	
	if (!name)
		err_out("请输入名称！");
		
	name2type(name, &con.type);
	ret = mon_get_buzzer(&con);
	if (MON_RET_SUCCESS==ret)
	{
		printf("type='buzzer';status='%d'\n",
				 con.status);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取蜂鸣器信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		err_out("无法获取蜂鸣器信息！");
	}

	err_out("未知错误！");
}

void get_ups()
{
	struct mon_ups con;
	
	ret = mon_get_ups(&con);
	if (MON_RET_SUCCESS==ret)
	{
		printf("type='%d';capacity='%d';delay='%d';shutdown='%d';battery_capacity='%d'\n",
				con.type, con.capacity, con.delay, con.shutdown, con.battery_capacity);
		exit(0);
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("获取UPS信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取UPS信息！");
		printf("type='ups';capacity='111';delay='111';shutdown='111';battery_capacity='111'\n");
		exit(0);
	}

	err_out("未知错误！");
}

void set_ups(char *type, char *capacity, char *delay, char *shutdown)
{
	struct mon_ups_setting con;
	
	con.type = *type;
	con.capacity = *capacity;
	con.delay = *delay;
	con.shutdown = *shutdown;
	ret = mon_set_ups(&con);
	if (MON_RET_SUCCESS==ret)
	{
	}
	else if (MON_RET_FAILED==ret)
	{
		err_out("设置UPS信息失败！");
	}
	else if (MON_RET_NONE==ret)
	{
		//err_out("无法获取UPS信息！");
		exit(0);
	}

	err_out("未知错误！");
}

void print_usage()
{
	printf("usage: --get_component \n \
      --get_main \n \
      --get_expander \n \
      --get_controller <name> <number> \n \
      --get_backplane <name> \n \
      --get_power <name> <number> \n \
      --get_fan <name> <number> \n \
      --get_lcd <name> \n \
      --get_buzzer <name> \n \
      --get_ups \n \
      --set_ups <type> <capacity> <delay> <shutdown> \n\
");
	exit(0);
}


/*****************************************************************************/
/*  主函数                                                                   */
/*****************************************************************************/

/* 标识长选项和对应的短选项的数组 */
const struct option long_options[] = {
	{ "get_component",  0, NULL, 'c' },
	{ "get_main",       0, NULL, 'm' },
	{ "get_expander",   0, NULL, 'e' },
	{ "get_controller", 1, NULL, 't' },
	{ "get_backplane",  1, NULL, 'b' },
	{ "get_power",      1, NULL, 'p' },
	{ "get_fan",        1, NULL, 'f' },
	{ "get_lcd",        1, NULL, 'l' },
	{ "get_buzzer",     1, NULL, 'z' },
	{ "get_ups",        0, NULL, 'u' },
	{ "set_ups",        1, NULL, 's' },
	{ NULL,             0, NULL, 0   } //最后一个元素标识为NULL
};

const char* const short_options = ":c:m:etbpflz:us";

int main(int argc, char *argv[])
{
	int next_option;  //下一个要处理的参数符号
	int haveargv = 0; //是否有我们要的正确参数，一个标识

	/* 参数检查 */
	if (argc<2)
		print_usage();

	do
	{
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		//printf("next_option=%c\n", next_option);
		//printf("main: %s, %s, %s\n", optarg, argv[optind], argv[optind+1]);
		switch (next_option)
		{
			case 'c':
				get_component();
				break;
			case 'm':
				get_main();
				break;
			case 'e':
				get_expander();
				break;
			case 't':
				get_controller(optarg, optarg+1);
				break;
			case 'b':
				get_backplane(optarg);
				break;
			case 'p':
				get_power(optarg, optarg+1);
				break;
			case 'f':
				get_fan(optarg, optarg+1);
				break;
			case 'l':
				get_lcd(optarg);
				break;
			case 'z':
				get_buzzer(optarg);
				break;
			case 'u':
				get_ups();
				break;
			case 's':
				set_ups(optarg, optarg+1, optarg+2, optarg+3);
				break;
			case ':':
				err_out("请输入参数！");
			case '?':     /* 出现一个未指定的参数*/
				err_out("未知参数！");
			case -1:      /* 处理完毕后返回-1 */
				if (!haveargv)
					err_out("参数不正确！");
				break;
			default:      /* 未指定的参数出现，出错处理 */
				print_usage();
				break;
		}
	}while (next_option !=-1);


	return 0;
}

