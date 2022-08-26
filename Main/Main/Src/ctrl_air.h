#ifndef CTRL_AIR_H
#define CTRL_AIR_H

#include "main.h"

#define NUMBER_REPEAT_SEND_CMD	2
#define NUMBER_CMD_CTRL			38
#define SHARP_AIR				1
#define DAIKIN_AIR_ARC433A46	2
#define DAIKIN_AIR_ARC423A5		3
#define REETECH_AIR				4
#define FUNIKI_AIR				5
#define MITSUMISHI_AIR		6
#define GENERAL_AIR				7
#define NAGAKAWA_LC				8 /* Cho loai khien cu */
#define NAGAKAWA_LM				9 /* Cho loai khien moi */
#define MITSUMISHI_HEAVY_INDUSTRIES		10
#define PANASONIC		11
#define REETECH_YKR		12
#define CARRIER				13
#define GREE					14
#define GALANZ				15
#define NAGAKAWA_R51M	16
#define SAMSUNG			  17
#define LG			  		18
#define TCL			  		19

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

#define SWING_VECTICAL			0
#define SWING_HORIZON				1

#define AIRCON_PORT                   GPIOB
#define AIRCON_PIN                    GPIO_PIN_12
#define AIRCON_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define AIR_HI()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_SET)
#define AIR_LO()                    HAL_GPIO_WritePin(AIRCON_PORT, AIRCON_PIN, GPIO_PIN_RESET)

#define TIME_CHECK_PASS 10 /* sensor read 1 time per second then 30 seconds equea 30seconds */

extern uint8_t error_air_module;
void start_condition_sharp(void);
void stop_condition_sharp(void);
void start_condition_reetech(void);
void stop_condition_reetech(void);
void start_condition_daikin(uint8_t daikin_id);
void restart_condition_daikin(uint8_t daikin_id);
void stop_condition_daikin(uint8_t daikin_id);
void control_air(uint8_t cmd, uint8_t air_id);
void control_air_sharp(void);
void control_air_reetech(void);
void control_air_daikin(void);
void start_condition_funiki(void);
void stop_condition_funiki(void);
void control_air_funiki(void);
void start_condition_mitsubishi(void);
void stop_condition_mitsubishi(void);
void control_air_mitsubishi(void);
void start_condition_general(void);
void stop_condition_general(void);
void control_air_general(void);
uint8_t do_getvalue(void);

void start_condition_nagakawa(void);
void stop_condition_nagakawa(void);

void control_air_nagakawa_lc(void);
void control_air_nagakawa_lm(void);
void control_air_mitsubishi_heavy_industries();

void start_condition_panasonic(void);
void stop_condition_panasonic(void);
void control_air_panasonic(void);

void control_air_tcl(void);
/*********************************************************************************************/
void set_temp(uint8_t temp);
void set_mode(uint8_t mode);
/*********************************************************************************************/
void set_request_check_air(void);
void clear_request_check_air(void);
uint8_t get_request_check_air(void);
uint8_t check_air_operation(int16_t temp_air_door);
void re_ctrl_air(void);

void start_condition_reeteck_ykr(void);
void stop_condition_reeteck_ykr(void);
void control_air_reeteck_ykr(void);

void start_condition_carrier(void);
void stop_condition_carrier(void);
void control_air_carrier (void);

void start_condition_gree(void);
void stop_condition_gree(void);
void restart_condition_gree(void);
void control_air_gree (void);

void start_condition_galanz(void);
void stop_condition_galanz(void);
void control_air_galanz(void);

void start_condition_nagakawa_r51m(void);
void stop_condition_nagakawa_r51m(void);
void control_air_nagakawa_r51m(void);

void start_condition_samsung(void);
void restart_condition_samsung(void);
void stop_condition_samsung(void);
void control_air_samsung(void);

void start_condition_lg(void);
void stop_condition_lg(void);
void control_air_lg(void);

void start_condition_tcl(void);
void stop_condition_tcl(void);

void start_condition_nagakawa_tq(void);
void stop_condition_nagakawa_tq(void);
void control_air_nagakawa_tq(void);
#endif /* DO_H */
