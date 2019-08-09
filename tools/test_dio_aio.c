
#include <fcntl.h>
#include <linux/aio_abi.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#define PFATAL(x...)                                                           \
	do {                                                                   \
		fprintf(stderr, "[-] SYSTEM ERROR : " x);                      \
		fprintf(stderr, "\n\tLocation : %s(), %s:%u\n", __FUNCTION__,  \
			__FILE__, __LINE__);                                   \
		perror("      OS message ");                                   \
		fprintf(stderr, "\n");                                         \
		exit(EXIT_FAILURE);                                            \
	} while (0)

inline static int io_setup(unsigned nr, aio_context_t *ctxp)
{
	return syscall(__NR_io_setup, nr, ctxp);
}

inline static int io_destroy(aio_context_t ctx)
{
	return syscall(__NR_io_destroy, ctx);
}

inline static int io_submit(aio_context_t ctx, long nr, struct iocb **iocbpp)
{
	return syscall(__NR_io_submit, ctx, nr, iocbpp);
}

inline static int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
			       struct io_event *events,
			       struct timespec *timeout)
{
	// This might be improved.
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}

#define BLOCKSZ (8192)

int main()
{
	int ret = 0;
	int fd = open("testaio.txt", O_CREAT | O_DIRECT | O_RDWR);
	ret = ftruncate(fd, 10*BLOCKSZ);
	if (ret < 0)
		printf("truncate error (%d)\n", ret);
	if (fd < 0) {
		PFATAL("open(testaio.txt)");
	}

	aio_context_t ctx = 0;
	int r = io_setup(128, &ctx);
	if (r < 0) {
		PFATAL("io_setup()");
	}

	void *bufw;
	void *bufr;
	posix_memalign(&bufw, 512, BLOCKSZ);
	posix_memalign(&bufr, 512, BLOCKSZ);
	memset(bufw, 'a', BLOCKSZ);
	struct iocb cb[2] = {
				{.aio_fildes = fd,
				.aio_lio_opcode = IOCB_CMD_PWRITE,
				.aio_buf = (uint64_t)bufw,
			  	.aio_nbytes = BLOCKSZ},
				{.aio_fildes = fd,
				.aio_lio_opcode = IOCB_CMD_PREAD,
				.aio_buf = (uint64_t)bufr,
				.aio_nbytes = BLOCKSZ},
			   };
		
	struct iocb *list_of_iocb[2] = {&cb[0], &cb[1]};

	r = io_submit(ctx, 2, list_of_iocb);
	if (r != 2) {
		PFATAL("io_submit()");
	}

	struct io_event events[2] = {};
	r = io_getevents(ctx, 2, 2, events, NULL);
	printf("ret val = %d\n", r);
	if (r != 2) {
		PFATAL("io_getevents()");
	}

	printf("read %lld bytes from testaio.txt\n", events[1].res);
	io_destroy(ctx);
	close(fd);
	if (memcmp(bufr, bufw, BLOCKSZ))
		PFATAL("memcmp()");
	else
		printf("memcmp passed\n");
	return 0;
}
