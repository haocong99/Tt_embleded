#ifndef DO_H
#define DO_H

#include "main.h"

#define AIRCON_PORT                   GPIOB
#define AIRCON_PIN                    GPIO_PIN_12
#define AIRCON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define AIR_HI()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_SET)
#define AIR_LO()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_RESET)

#define NO_ERROR_SENSOR			0x0e
#define NO_ERROR_AIR			  0x0d

#define WATCHDOG_PORT                   GPIOB
#define WATCHDOG_PIN                    GPIO_PIN_10
#define WATCHDOG_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define WATCHDOG_REFRESH()				HAL_GPIO_TogglePin(WATCHDOG_PORT,WATCHDOG_PIN)

#define TEMP_GROUP1 1
#define TEMP_GROUP2 2

void watchdog_ex_initialize(void);

void bit_0(void);
void bit_1(void);
void air_init(void);
void create_pulse(uint32_t number);
void sendbyte_data(uint8_t dat);
void bit_0_mitsubishi(void);
void bit_1_mitsubishi(void);

void bit_0_nagakawa(void);
void bit_1_nagakawa(void);
void sendbyte_data_nagakawa(uint8_t dat);

void sendbyte_data_mitsubishi(uint8_t dat);
void bit_0_group1_general(void);
void bit_1_group1_general(void);
void bit_0_group2_general(void);
void bit_1_group2_general(void);
void sendbyte_data_general(uint8_t dat, uint8_t group);

void bit_0_panasonic(void);
void bit_1_panasonic(void);
void sendbyte_data_panasonic(uint8_t dat);

void bit_0_reetech_ykr(void);
void bit_1_reetech_ykr(void);
void sendbyte_data_reetech_ykr(uint8_t dat);

void bit_0_carrier(void);
void bit_1_carrier(void);
void sendbyte_data_carrier(uint8_t dat);

void bit_0_galanz(void);
void bit_1_galanz(void);
void sendbyte_data_galanz(uint8_t dat);

void bit_0_nagakawa_r51m(void);
void bit_1_nagakawa_r51m(void);
void sendbyte_data_nagakawa_r51m(uint8_t dat);

void bit_0_samsung(void);
void bit_1_samsung(void);
void sendbyte_data_samsung(uint8_t dat);

void bit_0_lg(void);
void bit_1_lg(void);
void sendbyte_data_lg(uint8_t nibyte);

void bit_0_tcl(void);
void bit_1_tcl(void);
void sendbyte_data_tcl(uint8_t nibyte);
/*********************************************************************************************/
#endif /* DO_H */
