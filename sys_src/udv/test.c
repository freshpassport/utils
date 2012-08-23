#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "libudv.h"

void test_create()
{
	ssize_t ret;

	ret = udv_create("/dev/md1", "udv1", 15000000);
	if (ret!=0)
	{
		printf("ret = %ld\n", ret);
		exit(1);
	}
}

void test_list()
{
	udv_info_t list[MAX_UDV], *udv;
	size_t udv_cnt = 0, i;

	udv_cnt = udv_list(list, MAX_UDV);
	if (udv_cnt<=0)
	{
		printf("no udv found!\n");
		return;
	}

	printf("VG        NAME    NUM  START    END    CAPACITY\n");

	udv = &list[0];
	for (i=0; i<udv_cnt; i++)
	{
		printf("%-10s %-8s %-2d %-8lld %-8lld %-8lld\n",
			udv->vg_dev, udv->name, udv->part_num,
			udv->start, udv->end, udv->capacity);
		udv++;
	}
}

void test_delete()
{
	udv_delete("udv1");
}

int main()
{
	test_list();

	test_delete();

	test_list();

	return 0;
}
