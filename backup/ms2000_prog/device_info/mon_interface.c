#include <string.h>
#include "mon_interface.h"

int mon_get_component(struct mon_component *mc)
{
	mc->main_dev = 1;
	mc->expander = 0;
	mc->ups = 0;

	return 0;
}

int mon_get_main(struct mon_main *mm)
{
	strcpy(mm->name, "MS2000");
	strcpy(mm->sn, "Wisdata");
	mm->controller = 1;
	strcpy(mm->controller_info, "OCTEON");
	mm->power = 1;
	mm->fan = 1;
	mm->lcd = 0;
	mm->buzzer = 1;
	return 0;
}

int mon_get_expander(struct mon_expander *me)
{
	return MON_RET_NONE;
}

int mon_get_controller(struct mon_controller *mc)
{
	int i;

	if (mc->type != MT_MAIN)
		return MON_RET_NONE;
	strcpy(mc->sn, "Wisdata");
	mc->version = 1 << 16;
	strcpy(mc->cpu, "OCTEON");
	mc->mem = 2 * 1024;
	for (i = 0; i < sizeof(mc->temp); i++)
		mc->temp[i] = 26;

	for (i = 0; i < sizeof(mc->vol); i++)
		mc->vol[i] = 3 * 10;

	return 0;
}

int mon_get_backplane(struct mon_backplane *mb)
{
	int i;

	if (mb->type != MT_MAIN)
		return MON_RET_NONE;
	strcpy(mb->sn, "Wisdata");
	for (i = 0; i < sizeof(mb->temp); i++)
		mb->temp[i] = 26;
	for (i = 0; i < sizeof(mb->vol); i++)
		mb->vol[i] = 3 * 10;
	mb->mcu_ver = 1 << 16;

	return 0;
}

int mon_get_power(struct mon_power *mp)
{
	return MON_RET_NONE;
}

int mon_get_fan(struct mon_fan *mf)
{
	if (mf->type != MT_MAIN)
		return MON_RET_NONE;
	if (mf->number > 2 || mf->number < 0)
		return MON_RET_NONE;

	strcpy(mf->sn, "Wisdata");
	mf->max_speed = 6000;
	mf->current_speed = 3000;

	return 0;
}

int mon_get_lcd(struct mon_lcd *ml)
{
	return MON_RET_NONE;
}

int mon_get_buzzer(struct mon_buzzer *mb)
{
	mb->status = 0;
	return 0;
}

int mon_get_ups(struct mon_ups *mu)
{
	return MON_RET_NONE;
}

int mon_set_ups(struct mon_ups_setting *mu)
{
	return MON_RET_NONE;
}
