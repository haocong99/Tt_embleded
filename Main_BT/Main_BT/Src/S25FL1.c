#include "S25FL1.h"
#include "SPI.h"
#include "Time.h"
/************************************** Command for Set for S25FL164 ***************************/
uint8_t CMD_READ_FLASH_SR1      						  =  0x05; /* READY_STATUS content of SR1 register */
uint8_t CMD_READ_FLASH_SR2      						  =  0x35; /* READY_STATUS content of SR2 register */
uint8_t CMD_READ_FLASH_SR3     						  	  =  0x33; /* READY_STATUS content of SR3 register */
/*
The Write Enable command sets the Write Enable Latch (WEL) bit in the Status Register 
to a 1. The WEL bit must be set prior to every Page Program, Sector Erase, Block Erase, 
Chip Erase, Write Status Registers and Erase / Program Security Registers command.
*/
uint8_t CMD_WRITE_FLASH_ENABLE  						   =  0x06;
uint8_t CMD_WRITE_FLASH_DISABLE 						   =  0x04;

uint8_t CMD_WRITE_FLASH_ENABLE_VOLATILE_STATUS_REGISTER  =  0x50; /* Alow access to write SR1, SR2, SR3*/
uint8_t CMD_WRITE_FLASH_STATUS_REGISTER                  =  0x01; /* Alow write volatile to SR1, SR2, SR3*/

uint8_t CMD_WRITE_FLASH_PROGRAM_PAGE                     =  0x02; /* Allow write one byte to one page 256 byte */

uint8_t CMD_SECTOR_ERASE_FLASH                           =  0x20; /* Erase one sector 4KB */
uint8_t CMD_BLOCK_ERASE_FLASH                            =  0xD8; /* Erase one block 64KB */
uint8_t CMD_ERASE_ALL_FLASH							   	 =  0xC7; /* Erase full chip */
/***************************************** Command for Read *********************************************/
uint8_t CMD_READ_FLASH_DATA                              =  0x03; /* Allow read one byte or one page data*/
uint8_t CMD_READ_FLASH_FAST_DATA                         =  0x0B; /* Allow read data with double speed */
/**
  * @brief  This create a short time delay after changed status of CS pin.
  *    
  * @param  None
  * @retval None
  */
void delay_time_flash(void)
{
	for(uint8_t i = 0; i < 50 ;i++);
}
/**
  * @brief  This function check status of S25FL164
			by send command 0x05h to read SR1 register.
			Status of S25FL1 contain in the SR1[0]
			+ SR1[0] = 1 :Busy
			+ SR1[0] = 0 : Free (ready to use)
  *         
  * @param  None
  * @retval BUSY_STATUS or READY_STATUS
  */
int8_t S25FL1_IsReady(void)
{
	uint16_t count = 0;
	uint8_t result = 0x01;
//	uint8_t dummy_byte = 0xff; /* send dummy byte to create clock when read */
	/* Select chip by keep CS pin in low logic */
	FLASH_SPI_CS_ON() ;
	delay_time_flash();
	/* Send command read SR1 */
	Send_Data(&CMD_READ_FLASH_SR1,1, 0xff); 
	while((count < 1000) && (result & 0x01))
	{ 
		/* Read data */
		Read_Data(&result, 1, 0xff);
		count ++;
	}
	FLASH_SPI_CS_OFF();
	return ((result & 0x01)? BUSY_STATUS : READY_STATUS);
}
/**
  * @brief  This function set Protected Portion set none by reset Volatile bits SEC, TB, BP2, BP1, BP0
			in three register SR1, SR2, SR3.
			+ Step 1 : Check if SL25F164 busy, return busy status.
			+ Step 2 : Send command 0x50h allow access to write SR1, SR2, SR3.
			+ Step 3 : Send command 0x01h allow write volatile bits in SR1, SR2, SR3 and value to setups three register SR1, SR2, SR3.
			+ Step 4 : Wait SL25F164 ready again.
  *         
  * @param  None
  * @retval BUSY_STATUS or READY_STATUS
  */

int8_t S25FL1_Initialize(void)
{
	/* 
	 Protected Portion set bits SEC, TB, BP2, BP1, BP0
	 in three register SR1, SR2, SR3.
	*/
	/*|SRP0 |SEC  |TB   |BP2  |BP1  |BP0 |WEL  |BUSY |
	  |  0  |  1  |  0  |  0  |  0  | 0  |  0  |  0  |
	*/
	uint8_t SR1_data = 0x40;
	/*|SUS  |CMP  |LB3  |LB2  |LB1  |LB0 |QE   |SRP1 |
	  |  0  |  0  |  0  |  0  |  0  | 0  |  0  |  0  |
	*/
	uint8_t SR2_data = 0x00;
	/*|RFU  |W6   |W5   |W4   |LC3  |LC3 |LC2  |LC1  |
	  |  0  |  0  |  0  |  0  |  0  | 0  |  0  |  0  |
	*/
	uint8_t SR3_data = 0x00;
	/* Check status of flash chip */ 
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return BUSY_STATUS;
	}
	
	/* Select chip by keep CS pin in low logic */
	FLASH_SPI_CS_ON() ;
	/* Alow access to write SR1, SR2, SR3*/
	Send_Data(&CMD_WRITE_FLASH_ENABLE_VOLATILE_STATUS_REGISTER,1, 0xff);
	FLASH_SPI_CS_OFF();
	HAL_Delay(5); /* Delay 5ms */
	
	/* Select chip by keep CS pin in low logic */
	FLASH_SPI_CS_ON() ;
	/* Allow write volatile bits in SR1, SR2, SR3*/
	Send_Data(&CMD_WRITE_FLASH_STATUS_REGISTER,1, 0xff);
	Send_Data(&SR1_data,1, 0xff);
	Send_Data(&SR2_data,1, 0xff);
	Send_Data(&SR3_data,1, 0xff);
	FLASH_SPI_CS_OFF();
	
	/* Wait chip flag ready again */
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return BUSY_STATUS;
	} 
	return READY_STATUS;
}
/**
  * @brief  This function write one byte to SL25F164
			+ Step 1 : Check if address excess ending adreess (maximum address) of SL25F164?
			+ Step 2 : Check if SL25F164 busy, return busy status.
			+ Step 3 : Send command 0x06h to enable write (WLE =1);
			+ Step 4 : Send command 0x02h allow write page follow 3 byte address and 1 byte data.
			+ Step 5 : Wait SL25F164 ready again.
  *         
  * @param  None
  * @retval WRITE_ERROR_STATUS or BUSY_STATUS or WRITE_OK_STATUS
  */
int8_t S25FL1_Write_Byte(uint8_t data, uint32_t address)
{
	uint8_t add = 0;
	if(address > ENDING_ADDRESS_FLASH)
	{
		return WRITE_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_ENABLE,1, 0xff);
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Delay short time */
	delay_time_flash();
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON();
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_PROGRAM_PAGE,1, 0xff);
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);
    /* Send byte data */
	Send_Data(&data,1, 0xff);    
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Wait chip ready again */
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return WRITE_ERROR_STATUS;
	}
	return WRITE_OK_STATUS;
}



int8_t S25FL1_Program_Page(uint8_t *data, uint32_t address, uint16_t len)
{
	uint8_t  add = 0;
//	uint16_t  i;
	if(address > ENDING_ADDRESS_FLASH)
	{
		return WRITE_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_ENABLE,1, 0xff); 
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Delay short time */
	delay_time_flash();
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_PROGRAM_PAGE,1, 0xff); 
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);
	for(uint16_t count = 0; count < len; count ++ )
	{
		if(Send_Data(&data[count], 1 , 0x1ff) < 0)
		{
			FLASH_SPI_CS_OFF();
			return WRITE_ERROR_STATUS;    
		}
	}
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Wait chip ready again */
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return WRITE_ERROR_STATUS;
	}
	return WRITE_OK_STATUS;
}
/**
  * @brief  This function write one message to SL25F164
			+ Step 1 : Check if (address + lengh of message) excess ending adreess (maximum address) of SL25F164?
			+ Step 2 : Send one by one byte data in message to SL25F164.
  *         
  * @param  None
  * @retval WRITE_OK_STATUS or WRITE_ERROR_STATUS
  */

int8_t S25FL1_Write_Multi_Byte(uint8_t *data, uint32_t address, uint32_t len)
{
	
    uint16_t offset = 0;
    uint32_t temp = len;
    uint16_t byte_to_write = 0;
    offset = address % 256;
	if(address > ENDING_ADDRESS_FLASH)
	{
		return WRITE_ERROR_STATUS;
	}
    while(len)
    {
        temp = 256 - offset;
        byte_to_write = (temp >= len) ? len : temp;
		
        if(S25FL1_Program_Page(data, address, byte_to_write) != WRITE_OK_STATUS)
			return WRITE_ERROR_STATUS;
		
        address += byte_to_write;
        if (byte_to_write == len)
        {
            offset = temp - len; 
        }
        else
        {
            offset = 0;
        }
        data += byte_to_write;
        len -= byte_to_write;
    }
	return WRITE_OK_STATUS;
}
/**
  * @brief  This function read a byte from SL25F164
			+ Step 1 : Check if address excess ending adreess (maximum address) of SL25F164?
			+ Step 2 : Send command 0x03h to required read data from SL25F164.
			+ Step 3 : Send threes byte address whrere to read data from SL25F164.
			+ Step 5 : Read bytes from address.
  *         
  * @param  None
  * @retval READ_ERROR_STATUS or READ_OK_STATUS or BUSY_STATUS
  */

int8_t S25FL1_Read_Byte(uint8_t *data, uint32_t address)
{
	uint8_t add = 0;

	if(address > ENDING_ADDRESS_FLASH)
	{
		return READ_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable read to chip */
	Send_Data(&CMD_READ_FLASH_DATA,1, 0xff);
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);
	
	/*Delay short time */
	delay_time_flash();
	/* read data */
    Read_Data(data, 1, 0x1ff);     

	/* Read value */
	FLASH_SPI_CS_OFF();
	/*Wait chip ready again */
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return READ_ERROR_STATUS;
	}
	return READ_OK_STATUS;
}

/**
  * @brief  This function read a byte from SL25F164
			+ Step 1 : Check if address excess ending adreess (maximum address) of SL25F164?
			+ Step 2 : Send command 0x03h to required read data from SL25F164.
			+ Step 3 : Send threes byte address whrere to read data from SL25F164.
			+ Step 5 : Read bytes follow sequence.
  *         
  * @param  None
  * @retval READ_ERROR_STATUS or READ_OK_STATUS or BUSY_STATUS
  */


int8_t S25FL1_Read_Multi_Byte(uint8_t *data, uint32_t address, uint32_t len)
{
	
	uint8_t add = 0;
	if(address > ENDING_ADDRESS_FLASH)
	{
		return READ_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable read to chip */
	Send_Data(&CMD_READ_FLASH_DATA,1, 0xff);
	
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);
	
	/*Delay short time */
	delay_time_flash();	
	/* read data */
    Read_Data(data, len, 0x1ff);     

	/* Read value */
	FLASH_SPI_CS_OFF();
	/*Wait chip ready again */
	if(S25FL1_IsReady() != READY_STATUS)
	{
		return READ_ERROR_STATUS;
	}
	return READ_OK_STATUS;
}

/**
  * @brief  This function erase one Sector (4KB) from SL25F164
			+ Step 1 : Check if end address of sector excess number of sector on SL25F164?
			+ Step 2 : Check if SL25F164 busy, return busy status.
			+ Step 3 : Send command 0x20h to erase sector;
			+ Step 4 : Send three byte address of Sector want to erase.
			+ Step 5 : Wait erase finish.
  *         
  * @param  None
  * @retval ERASE_ERROR_STATUS or BUSY_STATUS or ERASE_OK_STATUS
  */

int8_t S25FL1_Erase_Sector(uint16_t Sector)
{
	uint32_t address = Sector * SECTOR_WIDTH; 
	uint8_t add;
	uint8_t count = 0;
	int8_t Flag_Busy = BUSY_STATUS;
	
	if( Sector > NUMBER_OF_SECTOR)
	{
		return ERASE_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
    /* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_ENABLE,1, 0xff);
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	
	/*Delay short time */
	delay_time_flash();
	
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable eraser to chip */
	Send_Data(&CMD_SECTOR_ERASE_FLASH,1, 0xff);
	
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);
	
	FLASH_SPI_CS_OFF();
	/*Delay time */
	while((Flag_Busy == BUSY_STATUS) && (count < 40))
	{
		HAL_Delay(20);
		Flag_Busy =  S25FL1_IsReady();
		count ++;
	}
	if ( count >= 40)
	{
		return ERASE_ERROR_STATUS; 
	}
	 return ERASE_OK_STATUS;	
}
/**
  * @brief  This function erase one Sector (4KB) from SL25F164
			+ Step 1 : Check if end address of sector excess number of sector on SL25F164?
			+ Step 2 : Check if SL25F164 busy, return busy status.
			+ Step 3 : Send command 0x20h to erase sector;
			+ Step 4 : Send three byte address of Sector want to erase.
			+ Step 5 : Wait erase finish.
  *         
  * @param  None
  * @retval ERASE_ERROR_STATUS or BUSY_STATUS or ERASE_OK_STATUS
  */
int8_t S25FL1_Erase_Block(uint16_t Block)
{
	uint8_t add;
	uint8_t count = 0;
	uint32_t address = Block * SECTOR_WIDTH; 
	int8_t Flag_Busy = BUSY_STATUS;
	
	if( Block > NUMBER_OF_BLOCK)
	{
		return ERASE_ERROR_STATUS;
	}
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
    /* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_ENABLE,1, 0xff);
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Delay short time */
	delay_time_flash();
	
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable eraser to chip */
	Send_Data(&CMD_BLOCK_ERASE_FLASH,1, 0xff);
	
	/* Send byte highest address */
	add = (uint8_t)((address >> 16) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte address */
	add = (uint8_t)((address >> 8) & 0xFF);
	Send_Data(&add,1, 0xff);
	/* Send byte lowest address */
	add = (uint8_t)(address & 0xFF);
	Send_Data(&add,1, 0xff);

	FLASH_SPI_CS_OFF();
	/*Delay time */
	while((Flag_Busy == BUSY_STATUS) && (count < 50))
	{
		HAL_Delay(20);
		Flag_Busy =  S25FL1_IsReady();
		count ++;
	}
	if ( count >= 50)
	{
		return ERASE_ERROR_STATUS; 
	}
	 return ERASE_OK_STATUS;	
}

/**
  * @brief  This function erase one Sector (4KB) from SL25F164
			+ Step 1 : Check if end address of sector excess number of sector on SL25F164?
			+ Step 2 : Check if SL25F164 busy, return busy status.
			+ Step 3 : Send command 0x20h to erase sector;
			+ Step 4 : Send three byte address of Sector want to erase.
			+ Step 5 : Wait erase finish.
  *         
  * @param  None
  * @retval ERASE_ERROR_STATUS or BUSY_STATUS or ERASE_OK_STATUS
  */

int8_t  S25FL1_Erase_All(void)
{
//	uint8_t add;
	uint16_t count = 0;
	int8_t Flag_Busy = BUSY_STATUS;
	/* Check status of flash chip */  
	if ( S25FL1_IsReady() == BUSY_STATUS)
	{
		return BUSY_STATUS;
	}
    /* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable write to chip */
	Send_Data(&CMD_WRITE_FLASH_ENABLE,1, 0xff);
	/* End of Cmd */
	FLASH_SPI_CS_OFF();
	/*Delay short time */
	delay_time_flash();
	/* Select chip by keep CS pin in low logic */
    FLASH_SPI_CS_ON() ;
	/* Enable eraser to chip */
	Send_Data(&CMD_ERASE_ALL_FLASH,1, 0xff);
	FLASH_SPI_CS_OFF();
	/*Delay time */
	while((Flag_Busy == BUSY_STATUS) && (count < 500))
	{
		HAL_Delay(20);
		Flag_Busy =  S25FL1_IsReady();
		count ++;
	}
	if ( count >= 500)
	{
		return ERASE_ERROR_STATUS; 
	}
	 return ERASE_OK_STATUS;	
}

