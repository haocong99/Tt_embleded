#ifndef BOOTLOADER_H
#define BOOTLOADER_H
#include "stdint.h"
#include "S25FL1.h"
/* ---------------------------------------------------------------------------- */

#define         No_Update                   0x00 /* No request update */
#define 		New_Update                  0x10 /* Request update */
#define 		Already_Update              0x20 /* Already update */

#define         Read_Error                  -1
#define         Read_Ok                      0

#define         Write_Error                 -1
#define         Write_Ok                     0

#define         Erase_Error                 -1
#define         Erase_Ok                     0

#define         Initial_OK                   0
#define         Initial_ERROR               -1

#define         Update_OK                    0
#define         Update_ERROR                -1

#define			UPDATE_SIZE_REQUEST_SECTOR	9 /* Sector thu 10 */
#define 		UPDATE_SIZE_ADD				0x9000
#define 		UPDATE_REQUEST_ADD			0x900A

#define			NUMBER_TRY_INIT				3

/* define for update */
#define         START_FLASH_INTERNAL_ADDRESS    0x08000000  /* Address of start internal flash */
#define 	    ADDRESS_APPLICATION	  			0x08004000 /* 16 Kbyte for bootloader */
#define         END_FLASH_IN_ADDRESS      	 	0x08010000  /* Address end of internal flash */
#define 		SIZE_OF_PACKAGE					64
#define         MAX_NUMBER_PACKAGE      		768 /* Address end of internal flash */
#define 		SECTOR_START_APP_STORE_EXT		10 /* bat dau tu sector 11 */
#define 		ADDR_START_APP_STOR_EXT			0xA000
#define 		NUMBER_PACKAGE_PER_SECTOR		64

typedef  void (*pFunction)(void);

/* Khai bao nguyen mau ham cho external flash*/
int8_t 	config_init(void);
int8_t 	request_update_firmware(void);
int8_t 	clear_request_update_firmware(void);
int8_t 	check_update(void);
int16_t get_size(void);
int8_t update_size(uint16_t number_package);
/* ham cho xoa flash ngoai */
int8_t eraser_sector_external_flash(uint16_t sector);
/* Khai bao nguyen mau ham cho internal flash*/
int8_t 	eraser_all_internal_flash_page(void);/*Erase all internal sector */
int8_t	download_internal_flash(uint8_t *buf,uint16_t package_number, uint8_t len);
int8_t  read_internal_flash(uint32_t address, uint8_t *buf, uint32_t len);
/* ham cho update firmware */
int8_t  save_firmware_external_flash(uint8_t *dat, uint16_t package);
int8_t  update_firmware_internal_flash(uint16_t package);
/* jumping to new application */
void jump_to_application(void);
#endif /*  BOOTLOADER_H */