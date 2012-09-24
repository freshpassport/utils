#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>

int dev2id(const char *short_dev, char *slot_id)
{
	char tmp_path[PATH_MAX], tmp_buf[PATH_MAX];
	char *tok, *bs;
	int len;

	assert(short_dev);
	assert(slot_id);

	sprintf(tmp_path, "/sys/block/%s", short_dev);
	if (readlink(tmp_path, tmp_buf, PATH_MAX-1)<0)
		return -1;
	tok = strstr(tmp_buf, "target");
	tok = strstr(tok, ":");
	tok++;
	bs = strstr(tok, "/");
	if ( tok && bs )
	{
		len = abs(bs - tok);
		strncpy(slot_id, tok, len);
		*(slot_id+len) = '\0';
		return 0;
	}

	return -1;
}

int main()
{
	char slot_id[128];
	int ret;

	ret = dev2id("sdb", slot_id);

	printf("ret=%d, %s\n", ret, slot_id);

}

#if 0
int main()
{
	char *str = "../devices/pci0000:00/0000:00:10.0/host2/target2:0:1/2:0:1:0/block/sdb";
	char *tok, *back_slash;
	int len;
	char target[128];

	tok = strstr(str, "target");
	back_slash = strstr(tok, "/");
	len = abs(back_slash - tok);
	strncpy(target, tok, len);
	target[len] = '\0';

	printf("tok = %s\n", tok);
	printf("sl = %s\n", back_slash);
	printf("t = %s\n", target);

	return 0;
}
#endif

