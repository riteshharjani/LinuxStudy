/*
 * Small test for checking fiemap functionality
 * Inspired from xfstests/src/fiemap-tester.c
 *
 * Sample Output
 * =============
 * 
 * [~/work/mytools/test]
 * riteshh-> ./a.out 
 * logical: [       0..       7] phys: 764046176..764046183 flags: 0x000 tot: 8
 * logical: [       8..      15] phys: 764046224..764046231 flags: 0x000 tot: 8
 * logical: [      16..      23] phys: 764043888..764043895 flags: 0x001 tot: 8
 * (0) extent flag = 0
 * (1) extent flag = 0
 * (2) extent flag = 1
 *
 * [~/work/mytools/test]
 * riteshh-> xfs_io -c "fiemap -v" testfile.txt 
 * testfile.txt:
 *  EXT: FILE-OFFSET      BLOCK-RANGE          TOTAL FLAGS
 *    0: [0..7]:          764046176..764046183     8   0x0
 *    1: [8..15]:         764046224..764046231     8   0x0
 *    2: [16..23]:        764043888..764043895     8   0x1
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/vfs.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/fiemap.h>

char *fname = "testfile.txt";
int blocks_to_map = 1;

static void
show_extent_block(struct fiemap_extent *extent, int blocksize)
{
	__u64	logical = extent->fe_logical;
	__u64	phys = extent->fe_physical;
	__u64	length = extent->fe_length;
	int	flags = extent->fe_flags;

	printf("logical: [%8llu..%8llu] phys: %8llu..%8llu "
	       "flags: 0x%03X tot: %llu\n",
		logical / blocksize, (logical + length - 1) / blocksize,
		phys / blocksize, (phys + length - 1) / blocksize,
		flags,
		(length / blocksize));
}

static void
show_extents(struct fiemap *fiemap, int blocksize)
{
	unsigned int i;
	blocksize = 512; /* to keep it same with xfs_io */

	for (i = 0; i < fiemap->fm_mapped_extents; i++)
		show_extent_block(&fiemap->fm_extents[i], blocksize);
}

int run_fiemap(int fd, int blocksize)
{
	struct fiemap *fiemap;
	char *fiebuf;
	int ret;

	fiebuf = malloc(sizeof(struct fiemap) + (blocks_to_map * sizeof(struct fiemap_extent)));
	if (!fiebuf) {
		printf("malloc failed\n");
		exit(1);
	}

	fiemap = (struct fiemap*) fiebuf;

	fiemap->fm_flags = FIEMAP_FLAG_SYNC;
	fiemap->fm_extent_count = blocks_to_map;
	fiemap->fm_mapped_extents = 0;

	fiemap->fm_start = 0;
	fiemap->fm_length = blocks_to_map * blocksize;

	ret = ioctl(fd, FS_IOC_FIEMAP, (unsigned long) fiemap);
	if (ret < 0) {
		printf("ioctl fiemap failed %d\n", ret);
		exit(1);
	}

	show_extents(fiemap, blocksize);
	for (int i = 0; i < fiemap->fm_mapped_extents; i++) {
		struct fiemap_extent *extent;
		extent = &fiemap->fm_extents[i];

		printf("(%d) extent flag = %d\n", i, extent->fe_flags);
	}
	free(fiebuf);
	return 0;
}

int main()
{
	int fd;
	int blocksize;
	fd = open(fname, O_RDWR | O_CREAT, 0644);
	if (fd < 0) {
		printf("Open failed\n");
		exit(1);
	}

	if (ioctl(fd, FIGETBSZ, &blocksize) < 0) {
		printf("ioctl of FIGETBSZ failed\n");
		close(fd);
		exit(1);
	}

	if (blocksize <= 0) {
		printf("wrong blocksize\n");
		close(fd);
		exit(1);
	}

	run_fiemap(fd, blocksize);
	close(fd);
	return 0;

}


/*
create a file with below cmds. 
1. fallocate -o 0 -l 8K testfile.txt
2. xfs_io -c "pwrite 8K 8K" testfile.txt
3. check extent mapping:- xfs_io -c "fiemap -v" testfile.txt
4. run this binary on with and without iomap patches:- ./a.out 

Withought patch:-
================
riteshh-> ./a.out 
logical: [       0..      15] phys: 332211376..332211391 flags: 0x800 tot: 16
(0) extent flag = 2048


With patch:-
============
riteshh-> ./a.out
logical: [       0..       7] phys: 122802736..122802743 flags: 0x801 tot: 8
(0) extent flag = 2049

*/
