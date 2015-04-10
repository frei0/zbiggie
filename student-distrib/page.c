#include "page.h"
#define PDE_PRESENT        0x1 
#define PDE_WRITE          0x2 
#define PDE_USER           0x4 
#define PDE_SIZE           0x80
#define PDE_ADDRESS_MASK 0xFFFFF000
#define PD_NUM_ENTRIES   1024
#define NUM_PDS 7
#define KERNEL_PD 0

#define PTE_PRESENT        0x1
#define PTE_WRITE          0x2
#define PTE_USER           0x4
#define PT_NUM_ENTRIES   1024

#define OFFSET_VIDEO   0xB8000

#define PAGING_PSE    0x10
#define PAGING_ENABLE 0x80000000

unsigned page_directories[NUM_PDS][PD_NUM_ENTRIES] __attribute__((aligned(OFFSET_4K))); //page directory 
unsigned low_memory_table[PT_NUM_ENTRIES] __attribute__((aligned(OFFSET_4K))); //first 4M, for mapping video

void init_paging(void){
	int i;
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[KERNEL_PD][i]=0x0; //fill page directory with non present entries


    for (i = 0; i < PT_NUM_ENTRIES; i++) 
		low_memory_table[i] = (i * OFFSET_4K); 

    page_directories[KERNEL_PD][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    low_memory_table[OFFSET_VIDEO/OFFSET_4K] = OFFSET_VIDEO | PDE_PRESENT;
    page_directories[KERNEL_PD][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
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
    : "r" (page_directories[KERNEL_PD]), "i" (PAGING_PSE), "i" (PAGING_ENABLE)
    : "ax", "cc","memory" 
    );
}

void init_pd(int pd_num)
{
	int i;
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[pd_num][i]=0x0; //fill page directory with non present entries
    page_directories[pd_num][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    page_directories[pd_num][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    page_directories[pd_num][32] = (pd_num+1)*OFFSET_4M | PDE_SIZE | PDE_PRESENT; //for program image
}

void set_cr3(int pd_num)
{
    asm volatile(
         "mov %0, %%cr3;      \
          "
    : 
    : "r" (page_directories[pd_num])
    : "ax", "cc","memory" 
    );
}
