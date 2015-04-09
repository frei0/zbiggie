#ifndef PAGE_H
#define PAGE_H
extern void init_paging(void);
extern void init_pd(int pd_num);
extern void set_cr3(int pd_num);

#endif

