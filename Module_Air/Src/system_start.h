#ifndef SYSTEM_START_H
#define SYSTEM_START_H
#include "main.h"
#include "bootloader.h"
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
	ERROR_EREASE_FLASH = 1,
	ERROR_WRITE_FLASH = 2,
	ERROR_READ_FLASH = 4,
	ERROR_FLASH_TIMEOUT = 8,
	ERROR_SENSOR = 16
};
#define SECTOR_TEST_FLASH 8
#define ADD_TEST_FLASH	  0x8000
#define NUMBER_TRY_READ	  5

#define SECTOR_UID_MODULE 7
#define ADD_UID_MODULE	  0x7000

#define MANUFACTURE_ID										0xEF//0xC8 // GD25Q16CTIGR
#define TYPE_DEVICE											0x40
#define CAPACITY_DEVICE										0x15


typedef union{
	uint32_t uid_value[3];
	uint8_t uid_byte[12];
}udi_t;

extern udi_t uid_mcu;
void get_uid_mcu(void);
uint8_t test_flash(void);
uint8_t test_sensor(void);
uint8_t get_error_system(void);
uint8_t test_system(void);
int8_t write_uid_module(uint8_t *uid_write,uint8_t *uid_read);
int8_t read_uid_module(uint8_t *uid_get);
int8_t read_information(flash_t *flash);
/*********************************************************************************************/
#endif /* DO_H */
