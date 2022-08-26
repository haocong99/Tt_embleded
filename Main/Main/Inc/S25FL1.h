#ifndef S25FL1_H
#define S25FL1_H
#include "stdint.h"
                                    /* Status of flash chip */
#define READY_STATUS                                        0 
#define BUSY_STATUS                                         -1 

#define SECTOR_STARTING_ADDRESS                             0x000000 /* Sector Starting Address */
#define ENDING_ADDRESS_FLASH                                0x1FFFFF /* Ending Address */

#define WRITE_OK_STATUS                                     0 
#define WRITE_ERROR_STATUS                                  -1 

#define READ_OK_STATUS                                      0 
#define READ_ERROR_STATUS                                   -1 

#define ERASE_OK_STATUS                                     0 
#define ERASE_ERROR_STATUS                                  -1 

#define PAGE_WIDTH                                          256 
#define NUMBER_OF_SECTOR                                    512 /* 0 -> 511 sector */
#define SECTOR_WIDTH                                        4096
#define BLOCK_WIDTH                                         65536
#define NUMBER_OF_BLOCK                                    	32 /* 0 -> 511 sector */

void 	delay_time_flash(void);

int8_t  S25FL1_IsReady(void);
int8_t  S25FL1_Initialize(void);

int8_t  S25FL1_Write_Byte(uint8_t data, uint32_t address);
int8_t  S25FL1_Program_Page(uint8_t *data, uint32_t address, uint16_t len);
int8_t  S25FL1_Write_Multi_Byte(uint8_t *data, uint32_t address, uint32_t len);

int8_t  S25FL1_Read_Byte(uint8_t *data, uint32_t address);
int8_t  S25FL1_Read_Multi_Byte(uint8_t *data, uint32_t address, uint32_t len);
int8_t  S25FL1_Erase_Sector(uint16_t Sector);
int8_t  S25FL1_Erase_Block(uint16_t Block);
int8_t  S25FL1_Erase_All(void);

#endif
