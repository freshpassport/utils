#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int ret, x, y;
	char *p;

	ret = sscanf(argv[1], "%d:%d", &x, &y);
	printf("dev = %s, ret = %d\n", argv[1], ret);

	p = strchr(argv[1], '/');
	printf("p = %s, %.8x\n", p, p);
	p = strchr(&p[1], '/');
	printf("p = %s, %.8x\n", p, p);
}

