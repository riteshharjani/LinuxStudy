#define _GNU_SOURCE                                                                 
#include <stdio.h>                                                                  
#include <sys/wait.h>                                                               
#include <fcntl.h>                                                                  
#include <stdlib.h>                                                                 
#include <assert.h>                                                                 
#include <unistd.h>                                                                 
#include <sys/mman.h>                                                               
                                                                                    
                                                                                    
static int pagesize;                                                                
                                                                                    
int main()                                                                          
{                                                                                   
        int *addr;                                                                  
        int *addr_child;                                                            
        pagesize = getpagesize();                                                   
                                                                                    
        addr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);    
        if (addr == MAP_FAILED) {                                                   
                printf("mmap failed\n");                                            
                exit(0);                                                            
        }                                                                           
        *addr = 1;                                                                  
                                                                                    
        switch(fork()) {                                                            
        case -1:                                                                    
                printf("fork failed\n");                                            
                exit(0);                                                            
        case 0:                                                                     
                printf("Child started, value = %d\n", *addr);                       
                addr_child = mremap(addr, pagesize, pagesize, MREMAP_FIXED | MREMAP_MAYMOVE, addr + 2 * pagesize);    
                if (addr_child == MAP_FAILED) {                                     
                        printf("mremap failed\n");                                  
                        exit(0);                                                    
                }                                                                   
                assert(addr_child != addr);                                         
                printf("Child value in remapped addr = %d\n", *addr_child);         
                (*addr_child)++;                                                    
                exit(0);                                                            
        default:                                                                    
                if (wait(NULL) == -1) {                                             
                        printf("wait failed");                                      
                        exit(0);                                                    
                }                                                                   
                printf("In parent, value = %d\n", *addr);                           
        }                                                                           
        return 0;                                                                   
}          

/*
Output:-
riteshh-> ./a.out 
Child started, value = 1
Child value in remapped addr = 1
In parent, value = 2
*/

