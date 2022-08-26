/*
 * sensors.h
 *
 *  Created on: April 1, 2021
 *      Author: ThuongTV
 */
#ifndef COMMON_H
#define COMMON_H
#include "stdio.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

void delay_us(uint32_t time);
void delay_ms(uint32_t time);

void    set_bit( uint8_t *array,  uint16_t k );
void    clear_bit( uint8_t *array,  uint16_t k );
uint8_t get_bit( uint8_t *array,  uint16_t k );

#endif
