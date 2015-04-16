#ifndef PAGE_H
#define PAGE_H
#define OFFSET_4M      0x400000
#define OFFSET_4K      0x1000
#define OFFSET_VIDEO   0xB8000

extern void init_paging(void);
extern void init_pd(int pd_num);
extern void set_cr3(int pd_num);

#endif

