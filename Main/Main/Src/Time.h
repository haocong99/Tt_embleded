#ifndef __TIME_H
#define __TIME_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include <stdint.h>
	 
#include "main.h"
#include "stm32f0xx_hal.h"
	 
#define TIMER3_PRESCALER 48000  /* Presscaler 48000 => 1 clocks/1ms */
#define TIMER3_PERIODE   999 	 /* Interval 1s */

#define TIMER16_PRESCALER 48     // Presscaler 48 => 48 clocks/1us
#define TIMER16_PERIODE   49999  // 50 ms

extern 	uint8_t package_timeout;
extern  uint8_t	turn_on_air_lost_connect;	 
void 	timer3_init(void);
void 	timer16_init(void);
void 	timer_init(void);
	 
void 	timer16_stop(void);
void 	timer16_resume(void);	 
	 
uint8_t process_timeover(void);
void 	delay_us(uint32_t us);
void   Delay(uint32_t x);

uint32_t get_time_s(void);
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
