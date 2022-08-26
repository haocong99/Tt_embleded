#include "do.h"
#include "time.h"
#include "stm32f0xx_hal_gpio.h"
#include "rs485.h"
#include "data_base.h"
#include "temperature.h"
#include "ctrl_air.h"

static uint32_t count = 0;
#define DATA_TEST		0xFF
bit_t bit = {{0},0};
uint8_t data = 0;
uint32_t time[30];
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
	
	/* Initialize pin read id 485 */
	ID485_DETERMINE_GPIO_CLK_ENABLE();
	HAL_GPIO_WritePin(ID485_DETERMINE_PORT, ID485_DETERMINE_PIN, GPIO_PIN_SET);
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = ID485_DETERMINE_PIN  ;
	HAL_GPIO_Init(ID485_DETERMINE_PORT , &GPIO_InitStructure);  		
	
}
void ID_module_air(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	INPUT_ID_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = INPUT_ID_PIN  ;
	HAL_GPIO_Init(INPUT_ID_PORT , &GPIO_InitStructure);  
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
	/* sau do delay_us mot hoang 492us */
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
	/* End bytes */
	create_pulse(15);	
}

void test_ir(void)
{
	uint16_t count  = 0;
	int8_t check_result = -1;
	do{
		sendbyte_data(DATA_TEST);
		count ++;
	}while( count < 400);
}
uint8_t get_id485_module(void)
{
	uint8_t jump_status = 0;
	jump_status = (HAL_GPIO_ReadPin(ID485_DETERMINE_PORT, ID485_DETERMINE_PIN) == GPIO_PIN_SET)?0:1;
	return jump_status;
}
uint8_t get_ID_module(void)
{
	uint8_t jump_status = 0;
	jump_status =(HAL_GPIO_ReadPin(INPUT_ID_PORT, INPUT_ID_PIN) == GPIO_PIN_SET)?0:1;
	return jump_status;
}