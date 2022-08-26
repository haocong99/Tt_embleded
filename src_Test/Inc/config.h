#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include "stdint.h"

	 void GPIO_Config(void);
	 void USART_Config(void);
	 void NVIC_Config(void);
	 void RCC_Config(void);
	 void TIM_Config(void);
	 void EXTI_Config(void);
	 void SPI_Config1(void);
	 void RTC_Config(void);
	 void IWDG_Configuration(void);
   uint8_t Check_Voltage(void);
   void ADC_Config(void);
	 /*-----------------------------------------------------------------------------------------------*/

	 
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

