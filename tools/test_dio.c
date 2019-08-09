#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BLOCKSZ 512

int main()
{
	int ret;
	int err;
	void* buf;
	void* buf2;

	posix_memalign(&buf, BLOCKSZ, BLOCKSZ);
	posix_memalign(&buf2, BLOCKSZ, BLOCKSZ);
	int fd = open("test2.txt", O_DIRECT | O_RDWR  | O_CREAT /*| O_TRUNC */);
	if (fd < 0) {
		printf("Error opening file %d\n", fd);
		return -1;
	}
	memset(buf, 'a', BLOCKSZ);
//	sprintf(buf, "This is a direct IO write test\n");
//	ret = ftruncate(fd, BLOCKSZ);

	lseek(fd, 0, SEEK_SET);
	if (ret < 0) {
		err = errno;
		printf("error in truncate %d %d\n", ret, err);
	}
	ret = write(fd, buf, BLOCKSZ);
	if (ret >= 0)
		printf("write was successfull %d\n", ret);
	else {
		err = errno;
		printf("error in write %d %d\n", ret, err);
		return -1;
	}

	lseek(fd, 0, SEEK_SET);
	ret = read(fd, buf2, BLOCKSZ);
	if (ret < 0) {
		err = errno;
		printf("error in read %d %d\n", ret, err);
		return -1;
	} else {
		printf("read was successfull %d\n", ret);
		if (memcmp(buf, buf2, BLOCKSZ))
			printf("But data does not match\n");
	}
	return 0;
}
