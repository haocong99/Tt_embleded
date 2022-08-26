#include "do.h"
#include "time.h"
#include "stm32f0xx_hal_gpio.h"
#include "rs485.h"
#include "data_base.h"
#include "temperature.h"
//#include "ctrl_air.h"
#include "sharp_air.h"
#include "DAIKIN_AIR.h"
#include "REETECH_AIR.h"
#include "FUNIKI_AIR.h"
#include "MITSUMISHI_AIR.h"
#include "NAGAKAWA_LC.h"
#include "NAGAKAWA_LM.h"
#include "MITSUMISHI_HEAVY_INDUSTRIES.h"
#include "PANASONIC.h"
#include "REETECH_YKR.h"
#include "CARRIER.h"
#include "GREE.h"
#include "GALANZ.h"
#include "NAGAKAWA_R51M.h"
#include "SAMSUNG.h"
#include "LG.h"
#include "TCL.h"


/**
  * @brief  Initial twos Aircondition
  *         In the begining the status of two Aircondition are off and wait turn on command from main board.
  * @param  None
  * @retval None
  */
void air_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    __GPIOB_CLK_ENABLE();
	
	/* initialize air 1*/
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = AIRCON_PIN  ;
	HAL_GPIO_Init(AIRCON_PORT , &GPIO_InitStructure);  	
	control_air(CMD_OFF_AIR,air_id);
	
	/* Initialize pin read id 485 */
	ID485_DETERMINE_GPIO_CLK_ENABLE();
	HAL_GPIO_WritePin(ID485_DETERMINE_PORT, ID485_DETERMINE_PIN, GPIO_PIN_SET);
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = ID485_DETERMINE_PIN  ;
	HAL_GPIO_Init(ID485_DETERMINE_PORT , &GPIO_InitStructure);  	
	
}
void watchdog_ex_initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 __GPIOB_CLK_ENABLE();
	/* initialize air 1*/
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = WATCHDOG_PIN  ;
	HAL_GPIO_Init(WATCHDOG_PORT , &GPIO_InitStructure); 
 	/* Khoi tao muc logic thap */
	HAL_GPIO_WritePin(WATCHDOG_PORT, WATCHDOG_PIN, GPIO_PIN_RESET);
}
void    create_pulse(uint32_t number)
{
	/* tao xung 37.83khz, duty 63.83 */
	uint32_t counter = 0;
	for(counter = 0; counter <= number; counter ++)
	{
		AIR_HI();
		delay_us(10);
		AIR_LO();
		delay_us(17);
	}
}
	
void bit_0(void)
{
	
	/* phat ra khoang 15 xung */
	create_pulse(15);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	delay_us(480);
}
void bit_1(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(15);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	delay_us(1430);
}
void sendbyte_data(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1();
		}
		else
		{
			bit_0();
		}
	}
}
void bit_0_mitsubishi(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(14);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	delay_us(395);
}
void bit_1_mitsubishi(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(14);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	delay_us(1158);
}
void sendbyte_data_mitsubishi(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_mitsubishi();
		}
		else
		{
			bit_0_mitsubishi();
		}
	}
}
/*
	18C,20C, 21C,23c, 27c, 29c, 30C
*/
void bit_0_group1_general(void)
{
	/* phat ra khoang 15 xung */
	/* logic 0 */
	create_pulse(16);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	/* Logic 1 */
	delay_us(310);
}
/*
	18C,20C, 21C,23c, 27c, 29c, 30C
*/
void bit_1_group1_general(void)
{
	/* phat ra khoang 15 xung */
	/* Logic 0 */
	create_pulse(16);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(1070);
}
void bit_0_group2_general(void)
{
	/* phat ra khoang 15 xung */
	/* logic 0 */
	create_pulse(14);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	/* Logic 1 */
	delay_us(355);	
}
void bit_1_group2_general(void)
{
	/* phat ra khoang 15 xung */
	/* Logic 0 */
	create_pulse(14);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(1025);	
}
void sendbyte_data_general(uint8_t dat, uint8_t group)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			if(group == TEMP_GROUP1)
			{
				bit_1_group1_general();
			}else{
				bit_1_group2_general();
			}
		}
		else
		{
			if(group == TEMP_GROUP1)
			{
				bit_0_group1_general();
			}else{
				bit_0_group2_general();
			}
		}
	}
}

void bit_0_nagakawa(void)
{
	/* Logic 0 */
	create_pulse(17);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(310);
}
void bit_1_nagakawa(void)
{
	/* Logic 0 */
	create_pulse(17);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(1000);
}
void sendbyte_data_nagakawa(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_nagakawa();
		}
		else
		{
			bit_0_nagakawa();
		}
	}
}
void bit_0_panasonic(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(15);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	delay_us(395);
}
void bit_1_panasonic(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(15);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	delay_us(1158);
}
void sendbyte_data_panasonic(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_panasonic();
		}
		else
		{
			bit_0_panasonic();
		}
	}
}

void bit_0_reetech_ykr(void)
{
	/* Logic 0 */
	create_pulse(22);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(497);
}
void bit_1_reetech_ykr(void)
{
	/* Logic 0 */
	create_pulse(22);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1500);
}
void sendbyte_data_reetech_ykr(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_reetech_ykr();
		}
		else
		{
			bit_0_reetech_ykr();
		}
	}
}

void bit_0_carrier(void)
{
	/* Logic 0 */
	create_pulse(17);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(310);
}
void bit_1_carrier(void)
{
	/* Logic 0 */
	create_pulse(17);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1500);
}
void sendbyte_data_carrier(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_carrier();
		}
		else
		{
			bit_0_carrier();
		}
	}
}

void bit_0_galanz(void)
{
	/* Logic 0 */
	create_pulse(20);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(434);
}
void bit_1_galanz(void)
{
	/* Logic 0 */
	create_pulse(20);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1030);
}
void sendbyte_data_galanz(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_galanz();
		}
		else
		{
			bit_0_galanz();
		}
	}
}


void bit_0_nagakawa_r51m(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(500);
}
void bit_1_nagakawa_r51m(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1500);
}
void sendbyte_data_nagakawa_r51m(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_nagakawa_r51m();
		}
		else
		{
			bit_0_nagakawa_r51m();
		}
	}
}
void bit_0_samsung(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(440);
}
void bit_1_samsung(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1310);
}
void sendbyte_data_samsung(uint8_t dat)
{
	uint8_t count = 0;
	for(count = 0; count < 8; count ++)
	{
		if((dat >> count) &0x01)
		{
			bit_1_samsung();
		}
		else
		{
			bit_0_samsung();
		}
	}
}
void bit_0_lg(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 1430us */
	/* logic 1 */
	delay_us(440);
}
void bit_1_lg(void)
{
	/* Logic 0 */
	create_pulse(18);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* logic 1 */
	delay_us(1380);
}
void sendbyte_data_lg(uint8_t nibyte)
{
	int8_t count = 0;
	for(count = 3; count >=0; count --)
	{
		if((nibyte >> count) &0x01)
		{
			bit_1_lg();
		}
		else
		{
			bit_0_lg();
		}
	}
}