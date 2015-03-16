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


void rtc_init(void);
#endif
