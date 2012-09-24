#include <stdio.h>
#include <unistd.h>

int main()
{
	return symlink("/tmp/disk/by-slot/0:1", "/tmp/disk/by-dev/sda");
}

