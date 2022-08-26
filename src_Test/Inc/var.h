#ifndef __var_H
#define __var_H

#include "std_header.h"
#include "prj_config.h"



#define DEFAULT_PASSWORD	"123456"
#define PRODUCT_PASSWORD	"111111"
#define FW_VERSION		"1.1.0"
//#define CONFIG_SIZE     252
#define SN_SIZE         10
#define ID_SIZE         15
#define PASSWORD_SIZE   6
#define DOMAIN_SIZE			32
#define TIME_SIZE	    	12
#define ADMIN_NUMBER	"+84981111111" // So ADMIN 



typedef struct {
	char fw_version[6];	
	char serial[SN_SIZE];
	char dev_id[ID_SIZE];
	char password[PASSWORD_SIZE];  
	uint16_t flash_code;	
	char host_domain[DOMAIN_SIZE];
	uint32_t host_port;
	char backup_domain[DOMAIN_SIZE];
	uint32_t backup_port;
	uint32_t Vsolar_high;
	uint32_t Vsolar_low;
	char phone1[14];
	char phone2[14];	
	char aphone1[14];
	char aphone2[14];	
//  uint8_t Night_Hour;	
//	uint8_t Day_Hour;
	uint8_t RTCorADC;   // Run mode RTC or user ADC Value.   ADC =0, RTC =1
	uint8_t control_led_mode;
//	uint32_t mode2_vbat_threshold;
//	uint8_t use_gsm;
	uint16_t is_config;
	uint16_t next_period_send_msg[TIME_SIZE]; // 4h30p = 270 min
	uint8_t led_cur_calib;
//	uint32_t period_gps_check;
	uint8_t distance;
	char lat[10];
	char lng[11];
	uint8_t gps_check_type;
	uint16_t next_gps_check;
} param_config_t;

extern param_config_t param;
extern char current_lat[10];
extern char current_lng[11];
extern uint8_t sms_gps_pos;

char * get_fw_version(void);
int32_t cfg_store_param(param_config_t param);
void get_device_info(char *info);
void cfg_get_param(void);
int32_t init_param(void);
//int32_t cfg_set_gsm(char *data);
//uint32_t cfg_get_use_gsm(void);
int32_t cfg_set_serial_number(char *data);
int32_t cfg_set_id(char *data);
int32_t cfg_flash_code (char *data);
int32_t cfg_Vsolar_high(char *data);
int32_t cfg_Vsolar_low(char *data);
int32_t cfg_set_time(char *data);
int32_t cfg_set_change_mode_cond(char *data);
uint32_t cfg_get_change_mode_cond(void);
//uint32_t cfg_get_night_hour(void);
//uint32_t cfg_get_day_hour(void);
uint32_t cfg_get_flashcode(void);
uint32_t cfg_get_Vsolar_low(void);
uint32_t cfg_get_Vsolar_high(void);
uint32_t cfg_get_Vbat_mode2(void);
char* cfg_get_password(void);
char* cfg_get_host_domain(void);
uint32_t cfg_get_host_port(void);
char* cfg_get_backup_domain(void);
uint32_t cfg_get_backup_port(void);
uint32_t cfg_get_led_control_mode(void);
char *cfg_get_serial(void);
char *cfg_get_id(void);
char *cfg_get_phone1(void);
char *cfg_get_phone2(void);
char *cfg_get_aphone1(void);
char *cfg_get_aphone2(void);
int32_t cfg_phone (char *data);
int32_t cfg_admin_number (char *data);
int32_t cfg_capacity (char *data);
int32_t cfg_blink_type (char *data);
int32_t cfg_passwd (char *data);
int32_t cfg_id (char *data);
int32_t cfg_time (char *data);
int32_t cfg_server (char *data);
int32_t cfg_backup_server (char *data);
char *cfg_get_capacity(void);
uint8_t cfg_get_led_calib(void);
int32_t cfg_led_calib (char *data);     
int32_t cfg_rst_pwd (char *data);
uint32_t cfg_get_period_gps_check(void);
int32_t cfg_set_period_gps_check(char *data);
int32_t cfg_position (char *data);
int32_t cfg_distance (char *data);
int32_t cfg_gps_check (char *data);
char *cfg_get_lat(void);
char *cfg_get_long(void);
uint8_t cfg_get_gps_check_type(void);
uint16_t cfg_get_next_gps_check(void);
//int32_t cfg_flash_config(char *data);
int8_t cfg_set_root_location(char *gps_buf);
int32_t cfg_fix_pos(char * data);
//int32_t cfg_set_gps_root(char *data);
#endif /*__ var_H */
