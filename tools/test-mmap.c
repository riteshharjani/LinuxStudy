/*

About test:-
1. Create 2 files. mmap-file, mmap-data.
2. mmap-file is a 2GB sparse file where at some random offsets of pagesize align, I try to write pagesize amount of known data pattern.
3. These offsets (which are pagesize align) are recorded into mmap-data 
file via normal read/write calls.
4. Then after we wrote to both files, we munmap the mmap-file and close both of these files. 
5. Then we do echo 3 > drop_caches. 
6. Then in the verify phase, using the offsets written in mmap-data file, I read the mmap-file to verify if it's contents are proper or not.
With that could not reproduce this issue.

By: Ritesh Harjani <ritesh.harjani@gmail.com>
*/

#include <stdio.h>    
#include <stdlib.h>    
#include <stdint.h>    
#include <unistd.h>    
#include <fcntl.h>    
#include <string.h>    
#include <ctype.h>                               
#include <assert.h>               
#include <time.h>
#include <sys/ioctl.h>    
#include <sys/types.h>    
#include <sys/stat.h>                        
#include <sys/statfs.h>                                              
#include <sys/vfs.h>    
#include <sys/mman.h>                                         
#include <linux/fs.h>                                               
#include <linux/types.h>                           
#include <linux/fiemap.h>    
                                          
char *fname = "mmap-file";
int pagesize = 4096;
unsigned long fsize = 2UL * 1024UL * 1024 * 1024;
char pattern = 0xab;
                                     
void run_test()                                             
{                                   
        int fd;
	int *bitset;
        char *buf, *addr;
	int bits, pos, cnt = 0;

	printf("Test write phase starting...\n");
	bits = fsize / pagesize;
	bitset = (int*) malloc(sizeof(int)*bits);
	assert(bitset != NULL);

	memset(bitset, 0, sizeof(int)*bits);
	assert(bitset != NULL);

	srand(time(0));
	while (cnt < (bits/10)) {
repeat:
		pos = rand() % bits;
		if (bitset[pos] == 1)
			goto repeat;
		bitset[pos] = 1;
		cnt++;
	}
             
        fd = open(fname, O_RDWR | O_CREAT, 0644);    
        if (fd < 0) {                                  
                printf("Open failed\n");    
                exit(1);                     
        }                                                                 

	assert(ftruncate(fd, fsize) == 0);    
                      
        addr = mmap(NULL, fsize, PROT_WRITE, MAP_SHARED, fd, 0);
        assert(addr != MAP_FAILED);

	for (int i = 0; i < bits; i++) {
		if (bitset[i] == 0) continue;
		memset(addr + (i * pagesize), pattern, pagesize);
	}

	//msync(addr, fsize, MS_SYNC);

        assert(munmap(addr, fsize) != -1);
        close(fd);

	fd = open("mmap-data", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd > 0);
	cnt = write(fd, bitset, sizeof(int) * bits);
	assert(cnt == (sizeof(int)*bits));
	fsync(fd);
	close(fd);
        free(bitset);
	printf("Test write phase completed...\n");
}

void run_verify()
{
	int fd, bits, cnt;
	int *bitset;
	char *buf;

	printf("Test verify phase starting...\n");
	buf = (char *)malloc(pagesize);
	assert(buf != NULL);

	bits = fsize / pagesize;
	bitset = (int*) malloc(sizeof(int)*bits);

	fd = open("mmap-data", O_RDONLY, 0644);
	assert(fd > 0);

	cnt = read(fd, bitset, sizeof(int)*bits);
	assert(cnt == (sizeof(int)*bits));
	close(fd);

	fd = open(fname, O_RDONLY, 0644);
	assert(fd > 0);
	for (int i = 0; i < bits; i++) {
		if (bitset[i] == 0) continue;
		lseek(fd, i * pagesize, SEEK_SET);
		read(fd, buf, pagesize);
		for (int i = 0; i < pagesize; i++) {
			assert(buf[i] == pattern);
			//printf("%d 0x%x\n", i, (char)buf[i]);
		}
	}
	free(bitset);
	free(buf);
	close(fd);
	printf("Test verify phase completed\n");
	printf("TEST PASSED\n");
}

int main(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, ":wvb")) != -1) {
		switch(opt) {
		case 'w':
			printf("Testing Write phase only\n");
			run_test();
			break;
		case 'v':
			printf("Testing Verify phase only\n");
			run_verify();
			break;
		case 'b':
			printf("Testing both phases\n");
			run_test();
			run_verify();
			break;
		case '?':
			printf("unknown option passed\n");
			break;
		}
	}
        return 0;
}
