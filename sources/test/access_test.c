#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	if (access("/tmp/disk", R_OK | W_OK | X_OK))
	{
		perror("access");
		printf("errno = %d\n", errno);
	}
}

