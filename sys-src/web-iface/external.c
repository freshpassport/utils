#include "web-iface.h"
#include "common.h"

int external_main(int argc, char *argv[])
{
	int i;
	char tmp[32], *p;
	char cmd[1024];

	strcpy(cmd, "sys-manager-py");
	for (i=0; i<argc; i++)
	{
		printf("argv[%d] = %s\n", i, argv[i]);
		if (!strncmp(argv[i], "--disk=", 7))
		{
			p = strstr(argv[i], "--disk=");
			printf("p = %s\n", p);
			strcpy(tmp, " --disk=\'");
			strcat(tmp, p);
			strcat(tmp, "\'");
		}
		else
			sprintf(tmp, " %s", argv[i]);
		strcat(cmd, tmp);
	}

	printf("cmd: \n\t%s\n", cmd);
	return system(cmd);
}
