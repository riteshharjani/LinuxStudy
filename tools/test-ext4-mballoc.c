/*
 * simple mmap write multithreaded test
 * By: Ritesh Harjani <ritesh.harjani@gmail.com>
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
#include <pthread.h>
                                          
struct thread_s {
	int id;
};

unsigned long fsize = 2.5 * 1024 * 1024;

void run_func(int id)
{                                   
        int fd;
	char fname[255] = {0};
	char *addr;

	sprintf(fname, "mmap-file-%d", id);
	printf("Test write phase starting file %s\n", fname);

	fd = open(fname, O_RDWR | O_CREAT, 0644);    
        if (fd < 0) {                                  
                printf("Open failed\n");    
                exit(1);                     
        }                                                                 

	assert(ftruncate(fd, fsize) == 0);    
                      
        addr = mmap(NULL, fsize, PROT_WRITE, MAP_SHARED, fd, 0);
        assert(addr != MAP_FAILED);

	for (int i = 0; i < fsize; i++) {
		addr[i] = 0xAB;
	}

	msync(addr, fsize, MS_SYNC);
	fsync(fd);
        assert(munmap(addr, fsize) != -1);
        close(fd);
	printf("Test write phase completed...\n");
}

void *thread_func(void *arg)
{
	struct thread_s *thread_info = (struct thread_s *)arg;
	pthread_t tid;

	tid = pthread_self();
	printf("Inside thread %lu %d\n", tid, thread_info->id);
	run_func(thread_info->id);
	return NULL;
}

void _run_test(int threads)
{
	pthread_t tid[threads];
	for (int i = 0; i < threads; i++) {
		struct thread_s *thread_info = (struct thread_s *) malloc(sizeof(struct thread_s));
		thread_info->id = i;
		assert(pthread_create(&tid[i], NULL, thread_func, thread_info) == 0);
	}
	for (int i = 0; i <threads; i++) {
		assert(pthread_join(tid[i], NULL) == 0);
	}
	return;
}

int main(int argc, char *argv[])
{
	int opt;
	int threads;
	while ((opt = getopt(argc, argv, ":t:")) != -1) {
		switch(opt) {
		case 't':
			threads = atoi(optarg);
			printf("Testing with (%d) threads\n", threads);
			_run_test(threads);
			break;
		case '?':
			printf("unknown option passed\n");
			break;
		}
	}
        return 0;
}
