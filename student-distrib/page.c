#include "page.h"
#define PDE_PRESENT        0x1 
#define PDE_WRITE          0x2 
#define PDE_USER           0x4 
#define PDE_SIZE           0x80
#define PDE_ADDRESS_MASK 0xFFFFF000

#define PDT_PRESENT        0x1
#define PDT_WRITE          0x2
#define PDT_USER           0x4

#define OFFSET_4M      0x400000
#define OFFSET_4K      0x1000
#define OFFSET_VIDEO   0xB8000

#define PAGING_PSE    0x10
#define PAGING_ENABLE 0x80000000

unsigned page_directory[1024] __attribute__((aligned(OFFSET_4K))); //page directory 
unsigned low_memory_table[1024] __attribute__((aligned(OFFSET_4K))); //first 4M, for mapping video

void init_paging(void){
	int i;
	for (i = 0; i < 1024; ++i)
		page_directory[i]=0x0; //fill page directory with non present entries


    for (i = 0; i < 1024; i++) 
		low_memory_table[i] = (i * OFFSET_4K); 

    page_directory[0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    low_memory_table[OFFSET_VIDEO/OFFSET_4K] = OFFSET_VIDEO | PDE_PRESENT;
    page_directory[1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    asm volatile(
         "mov %0, %%cr3;      \
                              \
          mov %%cr4, %%eax;   \
          or  %1,%%eax;       \
          mov %%eax,%%cr4;    \
                              \
          mov %%cr0, %%eax;   \
          or  %2, %%eax;      \
          mov %%eax, %%cr0;   \
          "
    : 
    : "r" (page_directory), "g" (PAGING_PSE), "g" (PAGING_ENABLE)
    : "ax", "cc","memory" 
    );
}
