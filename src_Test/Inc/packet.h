/*
 * packet.h
 *
 *  Created on: March 13, 2017
 *      Author: lehuu
 */
 
#ifndef __packet_H
#define __packet_H

#include "std_header.h"
#include "prj_config.h"

#define MAX_PACKET_SIZE		180

typedef enum {
	PERIODIC_MESSAGE = 1,
	MOVING_MESSAGE,
	BATTERY_MESSAGE,
	LIGHT_MESSAGE
} device_message_t;

#define START_FRAME_CHAR 		'#'
//#define DEVICE_CODE					"UX001368"
#define START_FRAME_CHARS     "!AIVDO,"
#define END_FRAME_CHAR       "\r\n"
//#define HDR_SIZE    13
//#define HDR_START_ADDR      0
//#define HDR_LEN_ADDR        2 
//#define HDR_LEN_SIZE        2       // 2 byte
//#define HDR_MSG_ID_ADDR         4
//#define HDR_MSG_ID_SZIE         7
//#define HDR_CMD_ADDR            11
//#define HDR_CMD_SIZE            2   //2 byte
//#define DATA_ADDR          13
uint8_t CalCheckSum(char *buf, uint8_t length);
//uint16_t creat_message_report(char *buf, uint8_t state, float vsolar, float vbat, uint32_t flash_code, uint8_t led_status, uint16_t gsm_signal);
//uint16_t creat_periodic_message(char *buf, char *gps_buf, float vbat, uint32_t flash_code);
uint16_t creat_periodic_message(char *buf, float vbat, uint32_t flash_code);
uint16_t creat_cinfo_message(char *buf, uint32_t flash_code);
uint16_t creat_led_message(char *buf);
uint16_t creat_bat_message(char *buf);
uint16_t creat_moving_message(char *buf, char *gps_buf);

#endif /*__packet_H */
