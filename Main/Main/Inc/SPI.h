#ifndef SPI_H
#define SPI_H

/* */
//#include "main.h"
#include "stdint.h"
#include "stm32f0xx_hal.h"

#define FLASH_SPI_CS_PORT                    GPIOA
#define FLASH_SPI_CS_PIN                     GPIO_PIN_4

#define FLASH_SPI_CLK_MOSI_PORT              GPIOA

#define FLASH_SPI_CLK_PIN                    GPIO_PIN_5
#define FLASH_SPI_MISO_PIN                   GPIO_PIN_6

#define FLASH_SPI_MOSI_PORT                  GPIOA
#define FLASH_SPI_MOSI_PIN                   GPIO_PIN_7

#define FLASH_SPI_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define FLASH_SPI_CS_ON()                     HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_OFF()                    HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET) 
/*************************************** Define for Initialize SPI  ************************************************/
#define SPI_Initial_False                     1 /* Initialize success */
#define SPI_Already                           0 /* Initialize false   */
/*************************************** Define for sent data from external flash **********************************/
#define SPI_Sent_Data_OK                      0 /* Sent success */
#define SPI_Sent_Data_ERROR                   -1 /* Sent false */
/*************************************** Define for read data from external flash **********************************/
#define SPI_Read_Data_OK                      0 /* Read success */
#define SPI_Read_Data_ERROR                   -1/* Read false */

uint8_t    	SPI_Initial(void); /* Initialize SPI */
int32_t 	Send_Data(uint8_t *pData, uint16_t Size, uint32_t Timeout); /* Send data to external flash  */
int32_t 	Read_Data(uint8_t *pData, uint16_t Size, uint32_t Timeout); /* Read data from external flash */

#endif /* SPI_H */
