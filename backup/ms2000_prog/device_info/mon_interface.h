#ifndef MON_INTERFACE_H
#define MON_INTERFACE_H

#include "types.h"

C_BEGIN_DECL

#define MON_MAX_NAME (32)
#define MON_MAX_SN_LEN (32)

#define MON_CTRL_TEMP_POINT (6)
#define MON_CTRL_VOL_POINT (6)

#define MON_BP_TEMP_POINT (3)
#define MON_BP_VOL_POINT (3)

enum {
	MON_RET_SUCCESS		= 0,	/* success */
	MON_RET_FAILED		= -1,	/* i/o error */
	MON_RET_NONE		= -2,	/* device not found */
};

struct mon_component {
	int	main_dev;
	int	expander;
	int	ups;
};

/* 主柜信息 */
struct mon_main {
	char	name[MON_MAX_NAME];
	char	sn[MON_MAX_SN_LEN];
	int		controller;
	char	controller_info[MON_MAX_NAME];
	int	power;
	int	fan;
	int	lcd;
	int	buzzer;
};

/* 类型。0：主控，1-4：扩展柜 */
enum MON_CONTROLLER_TYPE {
	MT_MAIN,
	MT_EXPANDER1,
	MT_EXPANDER2,
	MT_EXPANDER3,
	MT_EXPANDER4,
};

/* 扩展柜信息 */
struct mon_expander {
	int	type;
	char	name[MON_MAX_NAME];
	char	sn[MON_MAX_SN_LEN];
	int		controller;
	char	controller_info[MON_MAX_SN_LEN];
	int	power;
	int	fan;
	int	lcd;
	int	buzzer;
};

struct mon_controller {
	int	type;
	char	sn[MON_MAX_SN_LEN];
	int	version;
	char	cpu[MON_MAX_NAME];
	long	mem;			/* In MB */
	char	temp[MON_CTRL_TEMP_POINT];
	char	vol[MON_CTRL_VOL_POINT];
};

struct mon_backplane {
	int	type;
	char	sn[MON_MAX_SN_LEN];
	char temp[MON_BP_TEMP_POINT];	/* 单位1摄氏度 */
	char vol[MON_BP_VOL_POINT];	/* 单位100mV，即1表示100mV. 下同 */
	int	mcu_ver;
};

struct mon_power {
	int	type;
	int	number;
	char	sn[MON_MAX_SN_LEN];
	int	power;
	int	input_vol;
	int	output_vol;
};

struct mon_fan {
	int	type;
	int	number;
	char	sn[MON_MAX_SN_LEN];
	int	max_speed;
	int	current_speed;
};

struct mon_lcd {
	int	type;
	char	sn[MON_MAX_SN_LEN];
};

struct mon_buzzer {
	int	type;
	int	status;
};

struct mon_ups {
	int	type;
	int	capacity;
	int	delay;
	int	shutdown;
	int	battery_capacity;
};

struct mon_ups_setting {
	int	type;
	int	capacity;
	int	delay;
	int	shutdown;
};

/* function defination */

/* 获取组件列表 */
int mon_get_component(struct mon_component *mc);

/* 获取主柜信息 */
int mon_get_main(struct mon_main *mm);

/* 获取扩展柜信息 */
int mon_get_expander(struct mon_expander *me);

/* 获取控制器信息 */
int mon_get_controller(struct mon_controller *mc);

/* 获取背板信息 */
int mon_get_backplane(struct mon_backplane *mb);

/* 获取电源信息 */
int mon_get_power(struct mon_power *mp);

/* 获取风扇信息 */
int mon_get_fan(struct mon_fan *mf);

/* 获取LCD信息 */
int mon_get_lcd(struct mon_lcd *ml);

/* 获取蜂鸣器信息 */
int mon_get_buzzer(struct mon_buzzer *mb);

/* 获取UPS信息 */
int mon_get_ups(struct mon_ups *mu);

/* UPS设置 */
int mon_set_ups(struct mon_ups_setting *mu);

C_END_DECL

#endif
