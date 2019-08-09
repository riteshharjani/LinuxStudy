#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>

#define BLOCKSZ (1024 * 1024)
#define FILENAME "testfile"

void wait_for_input()
{
	printf("Enter to continue...\n");
	getchar();
	return;
}

int main()
{
	int err;
	int ret;
	struct stat st;
	off_t len = 1024 * 1024;
	off_t offset = 1024 * 1024;
	char *buf;
	int fd;

	fd = open(FILENAME, O_RDWR | O_CREAT);
	if (fd < 0) {
		err = errno;
		printf("Error opening file %d %d\n", fd, err);
		return -1;
	}

	/* buf alloc memaligned */
	buf = (char*) malloc(BLOCKSZ);
	memset(buf, 'a', BLOCKSZ);

	/* 100MB of write */
	for (int i = 0; i < 100; i++) {
		write(fd, buf, BLOCKSZ);
	}
	fsync(fd);
	fstat(fd, &st);
	free(buf);
	printf("file(%s), before punch-hole size(%lu), blocks(%lu)\n", FILENAME, st.st_size, st.st_blocks);

	wait_for_input();

	/* punch hole */
	for (int i = 0; i <= 100; i++) {
		ret = fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, offset, len);
		if (ret < 0) {
			err = errno;
			printf("fallocate failed %d %d\n", ret, err);
			exit(0);
		}
		offset += (2 * len);
	}
	fsync(fd); // is this needed?

	fstat(fd, &st);
	printf("file(%s), After punch-hole size(%lu), blocks(%lu)\n", FILENAME, st.st_size, st.st_blocks);
	close(fd);

	wait_for_input();

	/* DIO TEST */
	fd = open(FILENAME, O_RDWR | O_DIRECT);
	if (fd < 0) {
		err = errno;
		printf("open failed with O_DIRECT %d %d\n", fd, err);
		exit(0);
	}
	offset = 0;
	lseek(fd, 0, SEEK_SET);
	buf = valloc(BLOCKSZ);
	if (!buf) {
		err = errno;
		printf("valloc failed %d\n", err);
		exit(0);
	}
	memset(buf, 'a', BLOCKSZ);
	lseek(fd, (1024 * 1024 * 100) - 1024, SEEK_SET);
	for (int i = 1; i <= 100; i++) {
		ret = write(fd, buf, BLOCKSZ);
		printf("Data written iter(%d) at offset %lu\n", i, offset);

		wait_for_input();
		if (ret != BLOCKSZ) {
			err = errno;
			printf("write failed %d %d\n", ret, errno);
			exit(0);
		}
		offset += BLOCKSZ;
	}
	fsync(fd);
	fstat(fd, &st);
	printf("file(%s), After dio-write size(%lu), blocks(%lu)\n", FILENAME, st.st_size, st.st_blocks);
	close(fd);
	return 0;
}
