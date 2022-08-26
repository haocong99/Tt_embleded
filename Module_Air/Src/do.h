#ifndef DO_H
#define DO_H

#include "main.h"

#define AIRCON_PORT                   GPIOB
#define AIRCON_PIN                    GPIO_PIN_12
#define AIRCON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define AIR_HI()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_SET)
#define AIR_LO()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_RESET)

#define NO_ERROR_SENSOR			0xfe
#define NO_ERROR_AIR			0xfd
#define ERROR_SENSOR_TEMP		1
#define ERROR_AIR_OPERATION		2

#define ID485_DETERMINE_PORT          GPIOB
#define ID485_DETERMINE_PIN           GPIO_PIN_13
#define ID485_DETERMINE_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define WATCHDOG_PORT                   GPIOB
#define WATCHDOG_PIN                    GPIO_PIN_10
#define WATCHDOG_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define WATCHDOG_REFRESH()				HAL_GPIO_TogglePin(WATCHDOG_PORT,WATCHDOG_PIN)

/* Khai bao doc ve pin hong ngoai */
#define INPUT_IR_PORT                   GPIOB
#define INPUT_IR_PIN                    GPIO_PIN_13
#define INPUT_IR_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
/*Khai bao IDmodule*/
#define INPUT_ID_PORT                   GPIOA
#define INPUT_ID_PIN                    GPIO_PIN_8
#define INPUT_ID_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define IR_INPUT                         EXTI4_15_IRQn

#define BIT_0 0
#define BIT_1 1
 enum read_bit_step{
					 START_TIME = 0,	\
					 END_TIME};
typedef struct {
uint8_t data[50];
uint8_t len_data;
}data_ir_t;
typedef struct{
uint32_t time[100];
uint8_t number_event;
}bit_t;

void watchdog_ex_initialize(void);

void input_ir_init(void);
void input_ir_deinit(void);

void bit_0(void);
void bit_1(void);
void air_init(void);
void ID_module_air(void);
void create_pulse(uint32_t number);
void sendbyte_data(uint8_t dat);
void test_ir(void);
uint8_t get_id485_module(void);
uint8_t get_ID_module(void);
/*********************************************************************************************/
#endif /* DO_H */
