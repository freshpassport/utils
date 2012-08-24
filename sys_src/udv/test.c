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
			udv->geom.start, udv->geom.end, udv->geom.capacity);
		udv++;
	}
}

int main(int argc, char *argv[])
{
	if (argc!=2)
	{
		printf("input udv name to be deleted!\n");
		return -1;
	}

	printf("=========== before delete =============\n");
	test_list();

	udv_delete(argv[1]);

	printf("=========== after delete =============\n");
	test_list();

	return 0;
}
