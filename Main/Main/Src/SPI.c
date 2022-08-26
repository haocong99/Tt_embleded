#include "SPI.h"
#include "rs485.h"
#include "stm32f0xx_hal_spi.h"

SPI_HandleTypeDef hspi;
//SPI_HandleTypeDef hspi1;
GPIO_InitTypeDef GPIO_InitStructure;  
 /**
  * @brief  This function is configuration CS pin as ouput pin and high logic.
  * @param  None
  * @retval None
  */
uint8_t SPI_Initial(void)
{
	FLASH_SPI_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin   = FLASH_SPI_CS_PIN;
	HAL_GPIO_Init(FLASH_SPI_CS_PORT , &GPIO_InitStructure); 
  
	FLASH_SPI_CS_OFF();
	
	__HAL_RCC_SPI1_CLK_ENABLE();

	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin   = FLASH_SPI_CLK_PIN;
    GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
	HAL_GPIO_Init(FLASH_SPI_CLK_MOSI_PORT , &GPIO_InitStructure); 
	
	
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin   = FLASH_SPI_MISO_PIN;
    GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
	HAL_GPIO_Init(FLASH_SPI_CLK_MOSI_PORT , &GPIO_InitStructure); 
	
	
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin   = FLASH_SPI_MOSI_PIN;
    GPIO_InitStructure.Alternate = GPIO_AF0_SPI1;
	HAL_GPIO_Init(FLASH_SPI_MOSI_PORT , &GPIO_InitStructure); 	
	
	/*##-2- Configure the SPI peripheral ######################################*/
	/* Program the Mode, BidirectionalMode , Data size, Baudrate Prescaler, NSS
      management, Clock polarity and phase, FirstBit and CRC configuration in the hspi Init
      structure:
	  + Mode               : SPI_MODE_MASTER
	  + BidirectionalMode  : SPI_DIRECTION_2LINES
	  + Data size          : SPI_DATASIZE_8BIT
	  + Baudrate Prescaler : SPI_BAUDRATEPRESCALER_2 in stm32f0 Baudrate Prescaler >4 is not available.
	  + NSS management     : SPI_NSS_SOFT
	  + Clock polarity and phase : 11b (Mode 3) SCK stay at high logic.
	  + FirstBit           : SPI_FIRSTBIT_MSB
	  + SPI CRC Calculation: SPI_CRCCALCULATION_DISABLE
    */
	hspi.Init.Mode               =  SPI_MODE_MASTER;
	hspi.Init.Direction          =  SPI_DIRECTION_2LINES;
	hspi.Init.DataSize           =  SPI_DATASIZE_8BIT;
	hspi.Init.BaudRatePrescaler  =  SPI_BAUDRATEPRESCALER_256;
	hspi.Init.NSS                =  SPI_NSS_SOFT;
	hspi.Init.CLKPolarity        =  SPI_POLARITY_HIGH;
	hspi.Init.CLKPhase           =  SPI_PHASE_2EDGE;
	hspi.Init.FirstBit           =  SPI_FIRSTBIT_MSB;
	hspi.Init.CRCCalculation     =  SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial      = 	7;
	hspi.Instance                =  SPI1;
	/*##-3- Initialize the SPI registers by calling the HAL_SPI_Init() API*/
	/* 
    */ 
	if(HAL_SPI_Init(&hspi) != HAL_OK)
	{
		return SPI_Initial_False;
	}
	return SPI_Already;
}


 /**
  * @brief  This function send data via SPI
  * @param  uint8_t *pData : A pointer, point to buffer data will transfer.
	uint16_t Size  : Number of byte data will transfer. 
    uint32_t Timeout 
  * @retval None
  */
int32_t Send_Data(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	return (HAL_SPI_Transmit(&hspi, pData, Size, Timeout) == HAL_OK)? SPI_Sent_Data_OK : SPI_Sent_Data_ERROR;
}
 /**
  * @brief  This function read data via SPI
  * @param  uint8_t *pData : A pointer, point to buffer data will read.
	uint16_t Size  : Number of byte data will read. 
    uint32_t Timeout 
  * @retval None
  */
int32_t Read_Data(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{ 
	return ((HAL_SPI_Receive(&hspi, pData, Size, Timeout) == HAL_OK) ? SPI_Read_Data_OK : SPI_Read_Data_ERROR);
}
