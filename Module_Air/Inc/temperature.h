#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include <stdint.h>
	 
#include "main.h"
#include "stm32f0xx_hal.h"
	 
#define TEM_SS1_PORT	GPIOB
#define TEM_SS1_PIN		GPIO_PIN_7
	 
#define INPUT_SS_BIT()		HAL_GPIO_ReadPin(TEM_SS1_PORT, TEM_SS1_PIN)
#define TEM_SS_PIN_HIGH()	HAL_GPIO_WritePin(TEM_SS1_PORT, TEM_SS1_PIN, GPIO_PIN_SET)
#define TEM_SS_PIN_LOW()	HAL_GPIO_WritePin(TEM_SS1_PORT, TEM_SS1_PIN, GPIO_PIN_RESET)

#define TH_MAX			0x7f
#define TL_MIN			0x00
#define RESOLUTION_11B	0x0F

#define SKIP_ROM_CMD 		 0xCC	 
#define WRITE_SCRATCHPAD_CMD 0x4E	 
#define CONVERT_TEMP_CMD 	 0x44	 
#define CONVERT_TEMP_CMD 	 0x44
#define READ_TEMP_CMD		 0xBE

#define NUMBER_READ_FALSE	4
#define NUMBER_READ_OK		3

int16_t sensor_temp_init(void);
int16_t sensor_get_data(int16_t *temp);
int16_t tempearture_read(void);
int32_t get_temperature(void);
int32_t get_error(void);
uint32_t wait_state(uint8_t state, uint32_t timeout);
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
