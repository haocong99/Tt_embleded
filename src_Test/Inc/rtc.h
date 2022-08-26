#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include <time.h>
#include "stdint.h"
#include "stm32l071xx.h"
#include "stm32l0xx_hal_rtc.h"
	 
#define RTC_USART	USART1
#define TMO_RECV_SET	10000
#define TMO_WAIT_SYN	10000

#define RTC_TIMER_SECONDS	59 /* 1 min */	 
	 
#define RTC_CLOCK_SOURCE_LSI
/*#define RTC_CLOCK_SOURCE_LSE*/

#ifdef RTC_CLOCK_SOURCE_LSI
  #define RTC_ASYNCH_PREDIV    0x7C
  #define RTC_SYNCH_PREDIV     0x0127
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
  #define RTC_ASYNCH_PREDIV  0x7F
  #define RTC_SYNCH_PREDIV   0x00FF
#endif	 
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t date;
	uint8_t month;
	uint16_t year;
}rtc_time;
extern RTC_HandleTypeDef RtcHandle;

extern uint32_t recv_set_tmo;
extern uint32_t wait_syn_tmo;
extern void RTC_Init(void);
extern struct tm RTC_GetCalendar(void);
extern void RTC_SetCalendarTime(struct tm t);
uint32_t RTC_GetTimeInSec(void);
void RTC_TimeShow(void);
void MX_RTC_Init();
void read_rtc_time(void);
struct tm get_time(void);
void rtc_get_datetime(struct tm *curr_time);
	 /*-----------------------------------------------------------------------------------------------*/

	 
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

