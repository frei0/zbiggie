#include "page.h"
#include "lib.h"
#define PDE_PRESENT        0x1 
#define PDE_WRITE          0x2 
#define PDE_USER           0x4 
#define PDE_SIZE           0x80
#define PDE_ADDRESS_MASK 0xFFFFF000
#define PD_NUM_ENTRIES   1024
#define NUM_PDS 8
#define KERNEL_PD 0
#define USER_PAGE_DIR     32

#define PTE_PRESENT        0x1
#define PTE_WRITE          0x2
#define PTE_USER           0x4
#define PT_NUM_ENTRIES   1024


#define PAGING_PSE    0x10
#define PAGING_ENABLE 0x80000000

unsigned page_directories[NUM_PDS][PD_NUM_ENTRIES] __attribute__((aligned(OFFSET_4K))); //page directory 
unsigned low_memory_table[PT_NUM_ENTRIES] __attribute__((aligned(OFFSET_4K))); //first 4M, for mapping video
volatile int current_terminal = 0;
volatile int active_terminal = 10;

void init_paging(void){
	int i;
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[KERNEL_PD][i]=0x0; //fill page directory with non present entries


    for (i = 0; i < PT_NUM_ENTRIES; i++) 
		low_memory_table[i] = (i * OFFSET_4K); 

    page_directories[KERNEL_PD][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    low_memory_table[OFFSET_VIDEO/OFFSET_4K] = OFFSET_VIDEO | PTE_PRESENT |  PTE_WRITE | PTE_USER;
    page_directories[KERNEL_PD][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    page_directories[KERNEL_PD][NUM_PDS+1] = (NUM_PDS+1)*OFFSET_4M | PDE_SIZE | PTE_PRESENT |  PTE_WRITE;
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

void set_vmem_table(int term_num)
{
  //sets the nth vid_mem entry as present and user writable. 
  //if active, maps to real memory, otherwise to term_num's buffer.
  if (current_terminal == term_num)
      low_memory_table[OFFSET_VIDEO/OFFSET_4K] = OFFSET_VIDEO | PTE_PRESENT |  PTE_WRITE | PTE_USER;
  else
  {
      low_memory_table[OFFSET_VIDEO/OFFSET_4K] = ((NUM_PDS+1)*OFFSET_4M + term_num*OFFSET_4K) | PTE_PRESENT |  PTE_WRITE | PTE_USER;
  }

}

void init_pd(int pd_num)
{
	int i;
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[pd_num][i]=0x0; //fill page directory with non present entries
    page_directories[pd_num][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    page_directories[pd_num][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    page_directories[pd_num][USER_PAGE_DIR] = (pd_num+1)*OFFSET_4M | PDE_SIZE | PDE_PRESENT | PDE_USER | PDE_WRITE; //for program image
    page_directories[pd_num][USER_PAGE_DIR+1] = (((unsigned int) low_memory_table) & PDE_ADDRESS_MASK) | PDE_PRESENT | PDE_USER | PDE_WRITE;
    page_directories[pd_num][NUM_PDS+1] = (NUM_PDS+1)*OFFSET_4M | PDE_SIZE | PTE_PRESENT |  PTE_WRITE;
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

void switch_video(int term_num)
{
    if(current_terminal == term_num)
        return;
    memcpy((void *)((NUM_PDS+1)*OFFSET_4M + current_terminal*OFFSET_4K), (void*)OFFSET_VIDEO, OFFSET_4K);
    current_terminal = term_num;
    //todo: set vmem based on current pcb appropriately
    memcpy((void*)OFFSET_VIDEO,(void*)((NUM_PDS+1)*OFFSET_4M + term_num*OFFSET_4K),OFFSET_4K);
    switch_context(get_current_pid());
    /*if(active_terminal == term_num)
    {
        set_vmem_table(term_num);
    }*/
}

