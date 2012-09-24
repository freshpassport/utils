#include <stdio.h>
#include <stdlib.h>
#include <lvm2cmd.h>

void pv_create()
{
	int ret;
	char *lvm_handle = lvm2_init();
	if (!lvm_handle)
	{
		printf("init error\n");
		exit(1);
	}
	
	ret = lvm2_run(lvm_handle, "pvcreate /dev/sdb");
	printf("ret = %d\n", ret);

	lvm2_exit(lvm_handle);
}

int main()
{
	pv_create();
}

