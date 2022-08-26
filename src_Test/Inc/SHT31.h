/*
 * AMCSv1.0 - Copyright (C) 2016 ethings Ltd.
 * All rights reserved
 *
 * VISIT http://www.ethings.vn TO ENSURE YOU ARE USING THE LATEST VERSION.
 *
 * This file is part of the BTS.MON distribution.
 *
 * BTS.MON is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation >>>> AND MODIFIED BY <<<< the BTS.MON exception.
 
 * Author: hanv, quannv	
 */
 
#ifndef SHT31_H
#define SHT31_H

#include "stdint.h"
/* some define cmd */
#define SHT31_DEFAULT_ADDR    	   0x44
#define SHT31_MEAS_HIGHREP_STRETCH 0x2C06
#define SHT31_MEAS_MEDREP_STRETCH  0x2C0D
#define SHT31_MEAS_LOWREP_STRETCH  0x2C10
#define SHT31_MEAS_HIGHREP         0x2400
#define SHT31_MEAS_MEDREP          0x240B
#define SHT31_MEAS_LOWREP          0x2416
#define SHT31_READSTATUS           0xF32D
#define SHT31_CLEARSTATUS          0x3041
#define SHT31_SOFTRESET            0x30A2
#define SHT31_HEATEREN             0x306D
#define SHT31_HEATERDIS            0x3066
/* I2C define */

#define TEMP_HUMI_SS1_PORT 					GPIOB
#define IN_TEMP_HUMI_SS1_SDA_PIN 		GPIO_PIN_9
#define IN_TEMP_HUMI_SS1_SCL_PIN 		GPIO_PIN_8

#define I2Cx												I2C1

#define RESET_SHT31_PORT 				GPIOB
#define RESET_SHT31_PIN 				GPIO_PIN_7
#define ENABLE_SHT31()					HAL_GPIO_WritePin(RESET_SHT31_PORT, RESET_SHT31_PIN, GPIO_PIN_RESET)
#define DISABLE_SHT31()					HAL_GPIO_WritePin(RESET_SHT31_PORT, RESET_SHT31_PIN, GPIO_PIN_SET)

/* define a struct  */
typedef struct{
	int16_t humi;
  int16_t temper;
	uint8_t error;
	uint8_t count_error;
}sht31_data_t;
/* define some function  */
void I2C_begin(void);
uint8_t writeCommand(uint16_t cmd);
void soft_reset(void);
uint16_t readStatus(void);
void heater(uint8_t heater);
int8_t readTempHum();
void reset_internal_sensor(void);
void monitor_sensor(void);	
#endif /* AM2301_H */
