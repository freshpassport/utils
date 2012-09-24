#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "mon_interface.h"
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_TEMP      50 //最大温度值
#define MIN_TEMP 	   0 //最小温度值
#define MAX_VOL 	  50 //最大电压值
#define MIN_VOL 	   0 //最小电压值

#define SLEEP_TIME 60 //监控等待时间

struct name_type_conv{
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

void name2type(char *name, int *type){
	int i, items;
	
	if (NULL==name)
		return;
	if (NULL==type)
		return;
		
	items = (int)(sizeof(n2t)/sizeof(struct name_type_conv));
	
	for (i=0; i<items; i++)
	{
		if (!strcmp(name, n2t[i].dev_name))
		{
			*type = n2t[i].type;
			return;
		}
	}
}

struct Thread_Name {
	pthread_t thread;
	pthread_mutex_t mut;
};

void loginfo(char *msg)
{
	if (msg)
	{
		syslog(LOG_ERR, "wis_report: system EVENT_ERR %s\n", msg);
	}
}

static struct Thread_Name tn;

//执行检查的线程
void *thread1(void *name)
{
	pthread_mutex_lock(&tn.mut);
	
	int ret1, ret2, i;
	
	struct mon_controller con1;
	struct mon_backplane con2;
	
	name2type((char*)name, &con1.type);
	name2type((char*)name, &con2.type);

	while(1)
	{
		ret1 = mon_get_controller(&con1);

		if (MON_RET_SUCCESS==ret1)
		{
			for (i=0; i<MON_CTRL_TEMP_POINT; i++)
			{
				if ((MIN_TEMP<=con1.temp[i]) && (con1.temp[i]<=MAX_TEMP))
				{
				}
				else
				{
					//执行报警程序

					// 记录日志
					loginfo("控制器温度超出阀值! ");
				}
			}
			
			for (i=0; i<MON_CTRL_VOL_POINT; i++)
			{
				if ((MIN_VOL<=con1.vol[i]) && (con1.vol[i]<=MAX_VOL))
				{
				}
				else
				{
					//执行报警程序
					
					// 记录日志
					loginfo("控制器温度超出阀值! ");
				}
			}
		}
		else if (MON_RET_FAILED==ret1)
		{
			loginfo("获取组件列表失败！");
		}
		else if (MON_RET_NONE==ret1)
		{
			loginfo("无法获取组件列表！");
		}

		ret2 = mon_get_backplane(&con2);

		if (MON_RET_SUCCESS==ret2)
		{
			for (i=0; i<MON_BP_TEMP_POINT; i++)
			{
				if ((MIN_TEMP<=con2.temp[i]) && (con2.temp[i]<=MAX_TEMP))
				{
				}
				else
				{
					//执行报警程序
					
					loginfo("背板温度超出阀值! ");
				}
			}

			for (i=0; i<MON_BP_VOL_POINT; i++)
			{
				if ((MIN_VOL<=con2.vol[i]) && (con2.vol[i]<=MAX_VOL))
				{
				}
				else
				{
					//执行报警程序
					
					loginfo("背板温度超出阀值! ");
				}
			}
		}
		else if (MON_RET_FAILED==ret2)
		{
			loginfo("获取组件列表失败！");
		}
		else if (MON_RET_NONE==ret2)
		{
			loginfo("无法获取组件列表！");
		}

		pthread_mutex_unlock(&tn.mut);
		
		sleep(SLEEP_TIME);
	}
	
	pthread_exit(NULL);
}

//生成一个线程thread_check去执行
void thread_create(void)
{
	int tmp;
	
	//生成线程,用初始属性初始化互斥锁
	pthread_mutex_init(&tn.mut, NULL);
	char *name = "main";
	if ((tmp = pthread_create(&tn.thread, NULL, thread1, (void*)name)) != 0)
	{
		loginfo("线程创建失败 ");
	}
	else
	{
		loginfo("main线程创建成功 ");
	}
	
}

void thread_wait(void)
{
	if (tn.thread != 0)
	{
		pthread_join(tn.thread, NULL);
		loginfo("线程执行结束 ");
	}
}

int main(int argc, char *argv[])
{
	// 启动守护进程
	daemon(1, 1);
	// 关闭不需要的信号
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	// 打开日志
	openlog("daemonlogs", LOG_CONS | LOG_PID, 0);
	//创建线程
	thread_create();
	//等待线程
	thread_wait();
	// 关闭日志
	closelog();

	return 0;
}
