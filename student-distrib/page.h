#ifndef PAGE_H
#define PAGE_H
#define OFFSET_4M      0x400000
#define OFFSET_4K      0x1000
#define OFFSET_VIDEO   0xB8000

extern void init_paging(void);
extern void init_pd(int pd_num);
extern void set_cr3(int pd_num);
extern void set_vmem_table(int term_num);
extern void switch_video(int term_num);
volatile int current_terminal;
volatile int active_terminal;


#endif

