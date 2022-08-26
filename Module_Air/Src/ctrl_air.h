#ifndef CTRL_AIR_H
#define CTRL_AIR_H

#include "main.h"

#define NUMBER_REPEAT_SEND_CMD	2
#define NUMBER_CMD_CTRL			8
#define SHARP_AIR				1
#define DAIKIN_AIR_ARC433A46	2
#define DAIKIN_AIR_ARC423A5		3
#define REETECH_AIR				4

extern uint8_t air_id;

#define CMD_ON_AIR	1
#define CMD_OFF_AIR	 0

#define COOL_MODE	2
#define DRY_MODE	1
#define AUTO_MODE	0

#define FAN_AUTO	0
#define FAN_NIGHT	1

#define SWING_VECTICAL				0
#define SWING_HORIZON				1

#define AIRCON_PORT                   GPIOB
#define AIRCON_PIN                    GPIO_PIN_12
#define AIRCON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define AIR_HI()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_SET)
#define AIR_LO()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_RESET)


#define TIME_CHECK_TEMPERATURE 1800 /* sensor read 1 time per second then 1800 seconds equea 30 minutes */

extern uint8_t error_air_module;
void start_condition_sharp(void);
void stop_condition_sharp(void);
void start_condition_reetech(void);
void stop_condition_reetech(void);
void start_condition_daikin(uint8_t daikin_id);
void restart_condition_daikin(uint8_t daikin_id);
void stop_condition_daikin(uint8_t daikin_id);
void control_air(uint8_t cmd, uint8_t air_id);
void control_air_sharp(uint8_t cmd_air);
void control_air_reetech(uint8_t cmd_air);
void control_air_daikin(uint8_t cmd_air, uint8_t daikin_id);
uint8_t do_getvalue(void);
/*********************************************************************************************/
void set_temp(uint8_t temp, uint8_t id_air);
void set_mode(uint8_t mode, uint8_t id_air);
/*********************************************************************************************/
void set_request_check_air(void);
void clear_request_check_air(void);
uint8_t get_request_check_air(void);
uint8_t check_air_operation(int16_t temp_air_door);
#endif /* DO_H */
