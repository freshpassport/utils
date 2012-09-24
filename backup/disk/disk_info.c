#include <linux/hdreg.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

int isbigendian(){
  short i=0x0100;
  char *tmp=(char *)&i;
  return *tmp;
}

void trim(char *out, const char *in)
{
  int i, first, last;

  // Find the first non-space character (maybe none).
  first = -1;
  for (i = 0; in[i]; i++)
    if (!isspace((int)in[i])) {
      first = i;
      break;
    }

  if (first == -1) {
    // There are no non-space characters.
    out[0] = '\0';
    return;
  }

  // Find the last non-space character.
  for (i = strlen(in)-1; i >= first && isspace((int)in[i]); i--)
    ;
  last = i;

  strncpy(out, in+first, last-first+1);
  out[last-first+1] = '\0';
}

void swapbytes(char *out, const char *in, size_t n)
{
  size_t i;

  for (i = 0; i < n; i += 2) {
    out[i]   = in[i+1];
    out[i+1] = in[i];
  }
}


void formatdriveidstring(char *out, const char *in, int n)
{
  char tmp[65];

  n = n > 64 ? 64 : n;
#ifndef __NetBSD__
  swapbytes(tmp, in, n);
#else
  if (isbigendian())
    strncpy(tmp, in, n);
  else
    swapbytes(tmp, in, n);
#endif
  tmp[n] = '\0';
  trim(out, tmp);
}

int get_disk_info(const char *dev)
{
	struct hd_driveid hid;
	int fd, i;
	char model[64];
        uint8_t *buff = (uint8_t*)&hid;

	fd = open(dev, O_RDONLY);
	if (fd<0)
		perror("open");
	if (!ioctl(fd, HDIO_GET_IDENTITY, &hid))
		perror("ioctl");
        buff[0] = 0xa1;
        if (ioctl(fd, HDIO_DRIVE_CMD, &hid))
          perror("HDIO_DRIVE_CMD");

	close(fd);

	// result
	printf("\
model: %.40s\
fw rev: %.8s\
serial: %.20s\n",
	hid.model, hid.fw_rev, hid.serial_no);

	printf("model: ");
	for (i=0; i<40; i++)
		printf("%c ", hid.model[i]);
	printf("\n");

	printf("fw rev: ");
	for (i=0; i<8; i++)
		printf("%c", hid.fw_rev[i]);
	printf("\n");

	printf("vendor: %c %c %c %c %c %c\n",
		hid.vendor0, hid.vendor1, hid.vendor2,
		hid.vendor3, hid.vendor4, hid.vendor5);

	formatdriveidstring(model, (char*)&hid.model, 40);
	printf("model: %s\n", model);
}

int main(int argc, char *argv[])
{
	return get_disk_info(argv[1]);
}

