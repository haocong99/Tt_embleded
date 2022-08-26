#include "SPI.h"
#include "S25FL1.h"
#include "bootloader.h"
#include "String.h"
#include "rs485.h"

static uint32_t RamSource;
pFunction Jump_To_Application;
uint32_t JumpAddress;

int8_t config_init() /* Khoi tao phan cung can thiet GPIO de bao loi */
{
	/* Config SPI */
	if(SPI_Initial() != SPI_Already)
		return Initial_ERROR;
	
	/* Config S25FL164 */
	if(S25FL1_Initialize() != READY_STATUS)
		return Initial_ERROR;
	
	return Initial_OK;
}

/* ---------------------------------------------------------------------------- */

int8_t request_update_firmware(void) /* write request update */
{
	/* Write request update */
	if(S25FL1_Write_Byte(New_Update, UPDATE_REQUEST_ADD) != WRITE_OK_STATUS)
		return Write_Error;
	
	return Write_Ok;
}

int8_t clear_request_update_firmware(void)
{
	if(S25FL1_Erase_Sector((uint16_t)UPDATE_SIZE_REQUEST_SECTOR) != ERASE_OK_STATUS)
		return Erase_Error;
	
	return Erase_Ok;
}
/* ---------------------------------------------------------------------------- */
int8_t check_update() /* check xem can update code khong */
{
	uint8_t  c   = 0;
	/* Read request update */
	if(S25FL1_Read_Byte(&c,UPDATE_REQUEST_ADD) != READ_OK_STATUS)
		return Read_Error;
		
	if(c == New_Update) /* have new update */
		return New_Update;
	
	return No_Update;
}/* ---------------------------------------------------------------------------- */
int16_t get_size() /* Tra ve so luong goi tin */
{
	uint8_t  c[2]   = {0};
	uint16_t num_of_byte = 0;
	/* Read */
	for(int i = 0; i<2; i++)
	{
		if(S25FL1_Read_Byte(&c[i], UPDATE_SIZE_ADD +i) != READ_OK_STATUS)
		{
			num_of_byte = 0; /* return reset */
			return Read_Error;
		}
	}
	num_of_byte = c[0] | (c[1] << 8);
	return num_of_byte;
}

/* ---------------------------------------------------------------------------- */
int8_t update_size(uint16_t number_package) /* ghi lai so luong goi tin */
{
	uint8_t c = 0;
	/* Eraser before write  */
	if(eraser_sector_external_flash(UPDATE_SIZE_REQUEST_SECTOR) <0)
	{
		return Erase_Error;
	}
	/* Write size of firmware  */
	for(uint8_t i = 0; i<2; i++)
	{
		/* Lowest byte */
		c = (number_package & 0xff);
		
		if(S25FL1_Write_Byte(c,  UPDATE_SIZE_ADD + i) != WRITE_OK_STATUS)
			return Write_Error;
		/* hightest byte */
		number_package = number_package >> 8;
	}
	return Write_Ok;
}
/* Ham xoa sector */
int8_t eraser_sector_external_flash(uint16_t sector)
{
	/* xoa mot sector cua flash ngoai */
	return S25FL1_Erase_Sector(sector);
}
/* ---------------------------------------------------------------------------- */
int8_t 	eraser_all_internal_flash_page(void)/*Erase all internal sector */
{
	uint32_t point =0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress  = ADDRESS_APPLICATION;
	EraseInitStruct.NbPages = 45; /* Eraser all 45 page */
	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&EraseInitStruct,&point) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return Erase_Error;
	}
	HAL_FLASH_Lock();
	return Erase_Ok;
}

/* ---------------------------------------------------------------------------- */
/* @Brief: Ghi du lieu vao Flash trong
 */
int8_t download_internal_flash(uint8_t *buf,uint16_t package_number, uint8_t len)
{
	uint32_t Add_Start = ADDRESS_APPLICATION + package_number * SIZE_OF_PACKAGE;
	if(	package_number > MAX_NUMBER_PACKAGE)
	{
		return Write_Error;
	}
	HAL_FLASH_Unlock();
	  /* Clear all FLASH flags */  
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);   
	RamSource = (uint32_t)&buf[0];
	for (int j = 0;(j < SIZE_OF_PACKAGE);j += 4)
	{													 
		/* Program the data received into STM32F40x Flash */
		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Add_Start +j, *(uint32_t*)RamSource) != HAL_OK )
		{
			return Write_Error;
		}
		/* Verify data vua ghi */
		if (*(uint32_t*)(Add_Start +j) != *(uint32_t*)RamSource)
		{
			HAL_FLASH_Lock();
			return Write_Error;
		}
		
		RamSource += 4;
	}
	
	HAL_FLASH_Lock();
	
	return Write_Ok;
}
/********************************************************************************/
int8_t read_internal_flash(uint32_t address, uint8_t *buf, uint32_t len) /* Read mot vung nho trong flash */
{
	do{
	*buf = (uint8_t)(*(uint8_t*)(address)); /* pointer to where need read */
	buf ++;
	address ++; 
	}
	while(len --);
	return Read_Ok;
}
/**********************************************************************************************************************/
int8_t  save_firmware_external_flash(uint8_t *dat, uint16_t package)
{
	/* ghi ra flash ngoai */
	uint8_t  data_read[SIZE_OF_PACKAGE];
	uint16_t next_sector = (package / NUMBER_PACKAGE_PER_SECTOR) + SECTOR_START_APP_STORE_EXT + 1;
	uint32_t start_add = ADDR_START_APP_STOR_EXT + package * SIZE_OF_PACKAGE;
	uint8_t count = 0;
	debug_len =sprintf((char *)&debug_arr[0],"pakage: %d, next_sector = %d\n",package, next_sector);
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	/* write external flash */
	if(S25FL1_Write_Multi_Byte(dat, start_add, SIZE_OF_PACKAGE) < 0)
	{
		debug_len =sprintf((char *)&debug_arr[0],"Write pakage: %d false\n",package);
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		return Update_ERROR;
	}else{
		/* to do */
	}
	/* xoa buffer de doc lai du lieu */
	memset(data_read,0,SIZE_OF_PACKAGE);
	/* wait flash responds */
	delay_time_flash();
	/* doc lai firmware vua ghi */
	if(S25FL1_Read_Multi_Byte(&data_read[0], start_add ,SIZE_OF_PACKAGE) != READ_OK_STATUS)
	{
		debug_len =sprintf((char *)&debug_arr[0],"re-check read pakage: %d false\n",package);
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		return Update_ERROR;
	}else{
		for(count = 0; count <  SIZE_OF_PACKAGE; count ++)
		{
			if(dat[count] != data_read[count])
			{
				debug_len =sprintf((char *)&debug_arr[0],"re-check pakage: %d false\n",package);
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				return Update_ERROR;
			}else{
				/* to do */
			}
		}
	}	
	/* check neu can xoa sector tiep theo thi xoa luon*/
	if((package % NUMBER_PACKAGE_PER_SECTOR) == (NUMBER_PACKAGE_PER_SECTOR - 1))
	{
		debug_len =sprintf((char *)&debug_arr[0],"eraser next sector: %d \n",next_sector);
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		/* xoa sector tiep theo */
		if(S25FL1_Erase_Sector(next_sector) < 0)
		{
			debug_len =sprintf((char *)&debug_arr[0],"eraser next sector false \n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			return Update_ERROR;
		}else{
			/* to do */
			debug_len =sprintf((char *)&debug_arr[0],"eraser next sector done \n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
		}
	}else{
		/* to do */
	}
	return Update_OK;
}
/**********************************************************************************************************************/
int8_t  update_firmware_internal_flash(uint16_t package)
{
	uint8_t arr_firware[SIZE_OF_PACKAGE];
	memset(&arr_firware[0],0,SIZE_OF_PACKAGE);
	/* start reading */
	if(S25FL1_Read_Multi_Byte(&arr_firware[0], ADDR_START_APP_STOR_EXT + SIZE_OF_PACKAGE * package,SIZE_OF_PACKAGE) != READ_OK_STATUS)
	{
		return Update_ERROR;
	}
	/* download to the internal flash */
	if(download_internal_flash(arr_firware,package, SIZE_OF_PACKAGE) != Write_Ok)
	{
		return Update_ERROR;
	}
	return Update_OK;
}
/*************************************************************************************************************************/
void jump_to_application(void)
{
	if (((*(__IO uint32_t*)ADDRESS_APPLICATION) & 0x2FFE0000 ) == 0x20000000)
	{ 
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (ADDRESS_APPLICATION + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) ADDRESS_APPLICATION);
		Jump_To_Application();
	}
}
