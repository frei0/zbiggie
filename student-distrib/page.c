#include "page.h"
#include "page_asm.h"

unsigned page_directory[1024] __attribute__((aligned(4096)));
unsigned low_memory_table[1024] __attribute__((aligned(4096)));
unsigned kernel_memory_table[1024] __attribute__((aligned(4096)));


void init_paging(void){
	int i;
	for (i = 0; i < 1024; ++i)
		page_directory[i]=2;


    for (i = 0; i < 1024; i++)
		low_memory_table[i] = (i * 0x1000) | 3; 

    for (i = 0; i < 1024; i++)
		kernel_memory_table[i] = (0x100000 + i * 0x1000) | 3; 

    page_directory[0] = ((unsigned int)low_memory_table) | 3;
    page_directory[1] = ((unsigned int)kernel_memory_table) | 3;
    asm_enable_paging(page_directory);
}
