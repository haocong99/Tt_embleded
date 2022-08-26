#ifndef GALANZ_H
#define GALANZ_H

#include "main.h"

#define NUMBER_REPEAT_SEND_CMD	2
#define NUMBER_CMD_CTRL			32
#define GALANZ				15

extern uint8_t air_id;
extern uint8_t status_air;
extern uint32_t last_time_ctrl;
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

#define TIME_CHECK_PASS 10 /* sensor read 1 time per second then 30 seconds equea 30seconds */

extern uint8_t error_air_module;
void start_condition_galanz(void);
void stop_condition_galanz(void);
void control_air_galanz(void);
void control_air(uint8_t cmd, uint8_t air_id);
uint8_t do_getvalue(void);
/*********************************************************************************************/
void set_temp(uint8_t temp);
void set_mode(uint8_t mode);
/*********************************************************************************************/
void set_request_check_air(void);
void clear_request_check_air(void);
uint8_t get_request_check_air(void);
uint8_t check_air_operation(int16_t temp_air_door);
void re_ctrl_air(void);


#endif /* DO_H */
