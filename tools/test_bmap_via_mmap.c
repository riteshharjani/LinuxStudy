#include <stdio.h>    
#include <stdlib.h>    
#include <stdint.h>    
#include <unistd.h>    
#include <fcntl.h>    
#include <string.h>    
#include <ctype.h>                               
#include <assert.h>               
#include <sys/ioctl.h>    
#include <sys/types.h>    
#include <sys/stat.h>                        
#include <sys/statfs.h>                                              
#include <sys/vfs.h>    
#include <sys/mman.h>                                         
#include <linux/fs.h>                                               
#include <linux/types.h>                           
#include <linux/fiemap.h>    
                                          
char *fname = "bmaptest2";    
                                     
/*
 * make sure to change the device name currently using /dev/loop2
 * cmd:- for i in {0..10}; do sudo ./a.out; sudo rm bmaptest2; sudo sync; done
 */
                                  
void run_test()                                             
{                                   
        int blkcnt, i, block;    
        struct stat st;                            
        int fd, fd_bd;           
        int blocksize, block_check;             
        char *addr, *buf;         
        off_t offset;    
             
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
    
                            
        assert(ftruncate(fd, 3 * blocksize) == 0);    
                      
        if (fstat(fd, &st)) {    
                perror("fstat failed\n");    
                exit(1);    
        }    
        addr = mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
        assert(addr != MAP_FAILED);

        for (int i = 0; i < blocksize; i++) {
                addr[i] = 'x';
        }

        msync(addr, st.st_size, MS_SYNC);
        printf("\n\n=========Testing IOCTL FIBMAP=========\n\n");

        blkcnt = (st.st_size + blocksize - 1) / blocksize;
        printf("File size = %ld, blkcnt = %d, blocksize = %d\n",
                st.st_size, blkcnt, blocksize);

        for (i = 0; i < blkcnt; i++) {
                block = i;
                if (ioctl(fd, FIBMAP, &block)) {
                        perror("ioctl fibmap failed\n");
                        exit(1);
                }
                printf("%3d %10d\n", i, block);
                if (i == 0) {
                        block_check = block;
                        break;
                }
        }

        fd_bd = open("/dev/loop2", O_RDONLY, 0644);
        assert(fd_bd > 0);
        offset = block_check * blocksize;
        assert(lseek(fd_bd, block_check * blocksize, SEEK_SET) != -1);
        printf("Lseek at %ld %i\n", offset, block_check);
        buf = (char *)malloc(blocksize);
//      memset(buf, 0, blocksize);
        assert(read(fd_bd, buf, blocksize) == blocksize);

        for (int i = 0; i < blocksize; i++) {
                if (buf[i] != 'x') {
                        printf("Assertion failed at %d\n", i);
                        assert(0);
                }
                assert(buf[i] == 'x');
        }
        free(buf);      
        assert(munmap(addr, st.st_size) != -1);
        close(fd);
        close(fd_bd);
        printf("Test passed\n");
        
}

int main()
{
        run_test();
        return 0;
}
