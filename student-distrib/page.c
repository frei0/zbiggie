#include "page.h"
#include "tasking.h"
#include "lib.h"
#define PDE_PRESENT        0x1 
#define PDE_WRITE          0x2 
#define PDE_USER           0x4 
#define PDE_SIZE           0x80
#define PDE_ADDRESS_MASK 0xFFFFF000
#define PD_NUM_ENTRIES   1024
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
unsigned video_table[PT_NUM_ENTRIES] __attribute__((aligned(OFFSET_4K))); //buffers and a map to real memory;
volatile int current_terminal = 0;
volatile int active_terminal = 10;

/*
* void init_paging();
*   Inputs: none
*   Return: none
*   Function: Initializes paging for the kernel and system
*/
void init_paging(void){
	int i;
  //Clear out kernel's page directory
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[KERNEL_PD][i]=0x0; //fill page directory with non present entries

    //Set offsets inside low memory table.
    for (i = 0; i < PT_NUM_ENTRIES; i++) 
		low_memory_table[i] = (i * OFFSET_4K); 
    //Set kernel's first page to low_memory_table and put video memory inside low_memory table
    page_directories[KERNEL_PD][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    low_memory_table[OFFSET_VIDEO/OFFSET_4K] = OFFSET_VIDEO | PTE_PRESENT |  PTE_WRITE | PTE_USER;
    //Set the kernel to map to itself in virtual memory and have a video table for fake video memory past all programs
    page_directories[KERNEL_PD][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    page_directories[KERNEL_PD][NUM_PDS+1] = (((unsigned int)video_table) & PDE_ADDRESS_MASK) | PDE_PRESENT |  PTE_WRITE | PDE_USER;
    //Initialize the 4 relevant page tables inside video table as buffers for fake video memory for terminals 1, 2, and 3
    int term_num; for (term_num = 0; term_num < 3; term_num++)
       video_table[term_num] = ((NUM_PDS+1)*OFFSET_4M + term_num*OFFSET_4K) | PTE_PRESENT |  PTE_WRITE | PTE_USER;
    //Set the 4th entry to translate to physical video memory
    video_table[3] = OFFSET_VIDEO | PTE_PRESENT |  PTE_WRITE | PTE_USER;

    //Set up paging by moving and setting reigisters cr0, cr3. and cr4
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
    void * j;
    //Set the colors of all of the terminals
    for(i = 0; i < 3; i++)
    {
       set_vmem_table(i);
       j =  (void *)((NUM_PDS+1)*OFFSET_4M + i*OFFSET_4K);
       for(; (int)j < ((NUM_PDS+1)*OFFSET_4M + i*OFFSET_4K) + OFFSET_4K; j++)
       {
           if((int)j % 2 == 0)
               *((char*)j) = 0;
           else
               *((char*)j) = attribs[i];
       }
       
    }
    set_vmem_table(0);
}

/*
* void set_vmem_table(int term_num);
*   Inputs: term_num, the terminal number of which we want to set video memory
*   Return: none
*   Function: Set video memory to either real video memory or it's fake 
*             buffer based on the current active terminal and process.
*/
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
/*
* void init_pd();
*   Inputs: pd_num
*   Return: none
*   Function: Initializes the page directory for a new process
*/
void init_pd(int pd_num)
{
	int i;
	for (i = 0; i < PD_NUM_ENTRIES; ++i)
		page_directories[pd_num][i]=0x0; //fill page directory with non present entries
    //Set the first two pages to low memory and the kernel
    page_directories[pd_num][0] = (((unsigned int)low_memory_table) & PDE_ADDRESS_MASK)| PDE_PRESENT;
    page_directories[pd_num][1] = OFFSET_4M | PDE_SIZE | PDE_PRESENT;
    //Set the next two to be the user program and user video memory which is contained in low_memory_table
    page_directories[pd_num][USER_PAGE_DIR] = (pd_num+1)*OFFSET_4M | PDE_SIZE | PDE_PRESENT | PDE_USER | PDE_WRITE; //for program image
    page_directories[pd_num][USER_PAGE_DIR+1] = (((unsigned int) low_memory_table) & PDE_ADDRESS_MASK) | PDE_PRESENT | PDE_USER | PDE_WRITE;
    //Set the video buffers outside of the normal range of programs so the fake buffers can be accessed by all of the tables
    page_directories[pd_num][NUM_PDS+1] = (((unsigned int) video_table) & PDE_ADDRESS_MASK) | PDE_PRESENT ;
}

/*
* void set_cr3(int pd_num);
*   Inputs: none
*   Return: none
*   Function: Sets cr3 to point to the correct page directory based on the page directory number
*/
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

/*
* void switch_video(int term_num);
*   Inputs: term_num, switch and copy video memory based on the current terminal
*   Return: none
*   Function: Copy's video memory to its buffer and then copies the new terminal's
*             video memory from its buffer to video memory.
*/
void switch_video(int term_num)
{
    cli();
    //If we would switch to the current terminal do nothing
    if(current_terminal == term_num)
    {
        sti();
        return;
    }
    //Copy memory from video memory to the right buffer and switch the position of the cursor to the new terminal
    memcpy((void *)((NUM_PDS+1)*OFFSET_4M + current_terminal*OFFSET_4K), (void*)((NUM_PDS+1)*OFFSET_4M + 3*OFFSET_4K), OFFSET_4K);
    current_terminal = term_num;

    //Copy the new terminal's video buffer to real video memory then make sure the process is in the right context
    memcpy((void*)((NUM_PDS+1)*OFFSET_4M + 3*OFFSET_4K),(void*)((NUM_PDS+1)*OFFSET_4M + term_num*OFFSET_4K),OFFSET_4K);
    switch_context(get_current_pid());

    sti();
}

