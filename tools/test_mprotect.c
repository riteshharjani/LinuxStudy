 #define _GNU_SOURCE
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <errno.h>

static int pagesize;
#define SHELL_FMT "cat /proc/%ld/maps | grep zero"
#define CMD_SIZE (sizeof(SHELL_FMT) + 20) /* EXTRA SPACE FOR PID VALUE */

void get_proc_maps(char *str) {
	char cmd[CMD_SIZE];

	printf("%s: %s -- %ld\n", __func__, str, getpid());
	snprintf(cmd, CMD_SIZE, SHELL_FMT, (long) getpid());
	system(cmd);
}

int main()
{
	int *addr;
	int *addr_child;
	pagesize = getpagesize();

	struct rlimit rlim;

	if (getrlimit(RLIMIT_AS, &rlim) == -1) {
		printf("getrlimit failed %d\n", errno);
		exit(0);
	}

	printf("0x%llx - 0x%llx\n", rlim.rlim_cur, rlim.rlim_max);
	addr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED) {
		printf("mmap failed\n");
		exit(0);
	}
	*addr = 1;

	get_proc_maps("proc maps in parent");

	switch(fork()) {
	case -1:
		printf("fork failed\n");
		exit(0);
	case 0:
		printf("Child started, value = %d\n", *addr);
		get_proc_maps("Child: before mprotect");
		if (mprotect(addr, pagesize, PROT_NONE) == -1) {
			printf("mprotect failed %d\n", errno);
			exit(0);
		}
		get_proc_maps("Child: after mprotect");
/*
		void *newaddr = (void*)(addr + 2 * 4096);
		addr_child = mremap(addr, pagesize, pagesize, MREMAP_FIXED | MREMAP_MAYMOVE, newaddr);
		if (addr_child == MAP_FAILED) {
			int err = errno;
			printf("mremap failed %d\n", err);
			exit(0);
		}
		assert(addr_child != addr);
*/
		addr_child = addr;
//		printf("Child value in remapped addr = %d\n", *addr_child);
//		printf("addr = 0x%x, addr_child = 0x%x\n", addr, addr_child);
//		(*addr_child)++;
//		*(addr_child+1) = 8;
		while (1) {
			get_proc_maps("Child: after mprotect");
			sleep (2);
		}
		exit(0);
	default:
		while (1) {
			get_proc_maps("Parent: after mprotect");
			sleep (2);
		}
		if (wait(NULL) == -1) {
			printf("wait failed");
			exit(0);
		}
		printf("In parent, value = %d %d\n", *addr, *(addr+1));
	}
	return 0;
}

/*
Output:-
get_proc_maps: Parent: after mprotect -- 23568
7f32ee796000-7f32ee797000 rw-s 00000000 00:05 13227095                   /dev/zero (deleted)
get_proc_maps: Child: after mprotect -- 23572
7f32ee796000-7f32ee797000 ---s 00000000 00:05 13227095                   /dev/zero (deleted)
get_proc_maps: Parent: after mprotect -- 23568
7f32ee796000-7f32ee797000 rw-s 00000000 00:05 13227095                   /dev/zero (deleted)
get_proc_maps: Child: after mprotect -- 23572
7f32ee796000-7f32ee797000 ---s 00000000 00:05 13227095                   /dev/zero (deleted)
*/
