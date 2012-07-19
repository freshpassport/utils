#include <stdio.h>
#include <sys/types.h>

int main()
{
	char buf[1024];
	char *line_buf = NULL;
	size_t old_n = 1024;
	ssize_t n;
	int i;

	FILE *fp = popen("/usr/local/bin/sdmap", "r");
	//FILE *fp = fopen("sdmap.txt", "r");
	
	if (fp)
	{
		//fread(buf, 1024, 1, fp);
		//printf("return: \n%s\n", buf);
		n = getline(&line_buf, &old_n, fp);
		if (n > 0)
		{
			printf("line: %s\n", line_buf);
		}
		return pclose(fp);
		//return fclose(fp);
	}

	return -1;
}
