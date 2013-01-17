#include <stdio.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	char *pattern;
	DIR *dir;
	struct dirent *entry;
	int ret;

	dir = opendir(argv[2]);
	pattern = argv[1];

	printf("pattern: %s\n", pattern);

	if (!dir)
	{
		while( (entry=readdir(dir)) != NULL )
		{
			ret = fnmatch(pattern, entry->d_name, FNM_PATHNAME|FNM_PERIOD);
			if (ret == 0)
			{
				printf("%s\n", entry->d_name);
			}
			else if (ret == FNM_NOMATCH)
			{
				continue;
			}
			else
			{
				printf("error file: %s\n", entry->d_name);
			}
		}
		closedir(dir);
	}

	return 0;
}
