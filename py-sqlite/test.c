#include <stdio.h>

int main()
{
	char msg[128] = "test\0";

	printf("len = %d\n", strlen(msg));
}
