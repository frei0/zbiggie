#ifndef _RTC_H 
#define _RTC_H

//RTC ports
#define RTC_INDEX_PORT 0x70
#define RTC_RW_PORT    0x71

//NMI masks for 3 RTC ports
#define NO_NMI_A	   0x8A
#define NO_NMI_B	   0x8B
#define NO_NMI_C	   0x8C

//NMI unmask
#define NMI_ON		   0x7F

//PIC values for RTC
#define RTC_LINE	   8
#define RTC_ENABLE	   0x40

//Rate defines
#define MAX_RATE	   2
#define MIN_RATE	   15
#define RATE_MASK	   0xF0

#define HZ_2		   2
#define rate_HZ_2	   15
#define HZ_4		   4
#define rate_HZ_4	   14
#define HZ_8		   8
#define rate_HZ_8	   13
#define HZ_16		   16
#define rate_HZ_16	   12
#define HZ_32		   32
#define rate_HZ_32	   11
#define HZ_64		   64
#define rate_HZ_64	   10
#define HZ_128		   128
#define rate_HZ_128	   9
#define HZ_256		   256
#define rate_HZ_256	   8
#define HZ_512		   512
#define rate_HZ_512	   7
#define HZ_1024		   1024
#define rate_HZ_1024   6


void rtc_init(void);
int change_rtc_freq(int rate);
int rtc_read(void);
int rtc_write(unsigned int frequency);
int rtc_open(void);
int rtc_close(void);

#endif
