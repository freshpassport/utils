#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR *dp;
	struct dirent *fn;

	dp = opendir(argv[1]);
	if (!dp)
		return -1;

	while( (fn=readdir(dp)) != NULL )
	{
		if (!strcmp(fn->d_name, "."))
			continue;
		if (!strcmp(fn->d_name, ".."))
			continue;
		printf("f = %s\n", fn->d_name);
	}

	closedir(dp);
	return 0;
}

