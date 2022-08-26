#ifndef SYSTEM_START_H
#define SYSTEM_START_H
#include "temperature.h"
#include "main.h"
#include "bootloader.h"
#include "do.h"
#include "S25FL1.h"
#include "rs485.h"

/* voi module dieu hoa thi can phai kiem tra:
+ Flash.
+ Cam bien nhiet do.
+ Rs485 (Phan nay chua co cach kiem tra)
+ Led phat hong ngoai ( Phan nay chua co cach kiem tra)
*/
enum error_system{
	NO_ERROR = 0,
	ERROR_SENSOR_TEMP = 1,
	ERROR_AIR_OPERATION = 2,
	ERROR_FLASH = 4
};
#define SECTOR_TEST_FLASH 8
#define ADD_TEST_FLASH	  0x8000
#define NUMBER_TRY_READ	  5

#define SECTOR_UID_MODULE 7
#define ADD_UID_MODULE	  0x7000

typedef union{
	uint32_t uid_value[3];
	uint8_t uid_byte[12];
}udi_t;
extern uint8_t hardware_version[32];
extern uint8_t software_version[32];

extern udi_t uid_mcu;
void get_uid_mcu(uint32_t *uid);

uint8_t test_flash(void);
uint8_t test_sensor(void);
uint8_t get_error_system(void);
/*********************************************************************************************/
#endif /* DO_H */
