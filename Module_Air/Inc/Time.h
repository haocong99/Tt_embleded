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

	 
void 	timer3_init(void);
uint8_t process_timeover(void);
void 	delay_us(uint32_t us);
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
