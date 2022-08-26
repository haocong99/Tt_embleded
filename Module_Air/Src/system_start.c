#include "system_start.h"
#include "rs485.h"
#include "temperature.h"
#include "crc.h"
static uint8_t system_error = NO_ERROR;
udi_t uid_mcu;
char uid_mcu_arr[50];
char uid_flash_arr[50];
extern void ConvertUINT8ToHEXCHAR(uint8_t *temp, uint8_t num);
extern int32_t temp_read;
extern char module_uid[32];
extern char module_hardware[32];
flash_t 	flash;
/* Lay ra mcu id*/
void get_uid_mcu(void)
{
	uint8_t count  = 0;
	uint8_t len  = 0;
	
	uid_mcu.uid_value[0] = *(uint32_t *)UID_BASE;
	uid_mcu.uid_value[1] = *(uint32_t *)(UID_BASE + 0x04);
	uid_mcu.uid_value[2] = *(uint32_t *)(UID_BASE + 0x08);
	/* uid cua mcu */
	memset((uint8_t *)&uid_mcu_arr[0],0,50);
	for(count = 0; count < 12; count ++)
	{
		ConvertUINT8ToHEXCHAR((uint8_t *)&uid_mcu_arr[len + count * 2],uid_mcu.uid_byte[count]);
	}
}

/* Ham nay test flash bang cach xoa thu mot sector, ghi vao mot chuoi, 
doc ra chuoi do, so sanh voi chuoi ghi vao */
uint8_t test_flash(void)
{
	uint8_t i = 0;
	uint8_t arr_read[6] = {0,0,0,0,0};
	/* Check flash */
	if(S25FL1_IsReady() == BUSY_STATUS){
		/* Flash timeout */
		return ERROR_FLASH_TIMEOUT;
	}
	/* xoa flash */	
	S25FL1_Erase_Sector(SECTOR_TEST_FLASH);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Read_Byte(&arr_read[i], ADD_TEST_FLASH +i) != READ_OK_STATUS)
		{
			return ERROR_READ_FLASH;
		}else {
			if(arr_read[i] != 0xff) /* neu xoa khong duoc gia tri nay co the khac 0xff */
			{
				return ERROR_EREASE_FLASH;				
			}
		}
	}
	memset(&arr_read[0],0,6);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Write_Byte(i,  ADD_TEST_FLASH + i) != WRITE_OK_STATUS)
		{
			return ERROR_WRITE_FLASH;
		}
	}
	HAL_Delay(100);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Read_Byte(&arr_read[i], ADD_TEST_FLASH +i) != READ_OK_STATUS)
		{
			return ERROR_READ_FLASH;
		}else{
			if(i != arr_read[i])
			{
				return ERROR_READ_FLASH;
			}			
		}
	}
	return NO_ERROR;
}
uint8_t get_error_system(void)
{
	return system_error;
}
uint8_t test_system(void)
{
	char arr_start[50];
	char buffer[64];
	uint8_t len = 0;
	int8_t check_result;
	/* Check reset reason */
	if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)  != RESET)
	{
		/* Reset by watchdog ngoai pin ngoai */
		len = sprintf((char *)&arr_start[0],"AT+RESET=OK");
	}else{
		len = sprintf((char *)&arr_start[0],"AT+RESET=ERROR");
	}
	__HAL_RCC_CLEAR_RESET_FLAGS();
	/* debug */
	RS485_Respone((uint8_t *)&arr_start[0],len);
	/* Doc UID cua mcu */
	get_uid_mcu();
	memset(&buffer[0],'F',32);
	check_result = test_flash();
	if(check_result == NO_ERROR){
		/* Thong tin flash */
		read_information(&flash);	
	}else{
		flash.status = 0;
		return 1;
	}	
	system_error += test_sensor();
	/* Doc serial hien tai */
	memset(&buffer[0],0,64);
	read_uid_module((uint8_t*)&buffer[0]);
	memcpy(&module_uid[0],&buffer[0],32);
	memcpy(&module_hardware[0],&buffer[32],32);	
	return 0;
}
int8_t write_uid_module(uint8_t *uid_write,uint8_t *uid_read)
{
	uint8_t i = 0;
	uint8_t crc_cal = 0;
	uint8_t crc_read = 0;
	//memset(&uid_read[0],'F',64);
	/* Check flash */
	if(S25FL1_IsReady() == BUSY_STATUS){
		/* Flash timeout */
		return ERROR_FLASH_TIMEOUT;
	}	
	crc_cal = Cal_CRC8(&uid_write[0],64);
	/* xoa flash */	
	S25FL1_Erase_Sector(SECTOR_UID_MODULE);
	for(i = 0; i< 64; i++)
	{
		if(S25FL1_Write_Byte(uid_write[i],  ADD_UID_MODULE + i) != WRITE_OK_STATUS)
		{
			return ERROR_WRITE_FLASH;
		}
	}
	/* write crc */
	if(S25FL1_Write_Byte(crc_cal,  ADD_UID_MODULE + i) != WRITE_OK_STATUS)
	{
		return ERROR_WRITE_FLASH;
	}	
	HAL_Delay(100);
	for(i = 0; i< 64; i++)
	{
		if(S25FL1_Read_Byte(&uid_read[i], ADD_UID_MODULE +i) != READ_OK_STATUS)
		{
			return ERROR_READ_FLASH;
		}else{
			if(uid_read[i] != uid_write[i])
			{
				memset(&uid_read[0],'F',64);
				return ERROR_READ_FLASH;
			}			
		}
		
	}
	if(S25FL1_Read_Byte(&crc_read, ADD_UID_MODULE +i) != READ_OK_STATUS)
	{
		return ERROR_READ_FLASH;
	}else{
		crc_cal = Cal_CRC8(&uid_read[0], 64);
		if(crc_read != crc_cal)
		{
			memset(&uid_read[0],'F',64);
			return ERROR_READ_FLASH;
		}			
	}	
	return NO_ERROR;
}


/* Khoi tao va doc thu sensor */
uint8_t test_sensor(void)
{
	uint8_t count_test = 0;
	uint8_t error_read_sensor = ERROR_SENSOR;
	while((count_test < NUMBER_TRY_READ) && (error_read_sensor == ERROR_SENSOR))
	{
		if( sensor_get_data((uint16_t *)&temp_read) == 0)
		{
			error_read_sensor = NO_ERROR;
			break;
		}else{
			/* to do */
		}
		count_test ++;
	}
	return error_read_sensor;
}
int8_t read_uid_module(uint8_t *uid_get)
{
	uint8_t i = 0;
	uint8_t crc_read = 0;
	uint8_t crc_cal = 0;
	memset(&uid_get[0],'F',64);
	/* Check flash */
	if(S25FL1_IsReady() == BUSY_STATUS){
		/* Flash timeout */
		return ERROR_FLASH_TIMEOUT;
	}
	for(i = 0; i<64; i++)
	{
		if(S25FL1_Read_Byte(&uid_get[i], ADD_UID_MODULE +i) != READ_OK_STATUS)
		{
			memset(&uid_get[0],'F',64);
			return ERROR_READ_FLASH;
		}
	}
	if(S25FL1_Read_Byte(&crc_read, ADD_UID_MODULE +i) != READ_OK_STATUS)
	{	
		return ERROR_READ_FLASH;
	}else{
		crc_cal = Cal_CRC8(&uid_get[0], 64);
		if(crc_read != crc_cal)
		{			
			memset(&uid_get[0],'F',64);
			return ERROR_READ_FLASH;
		}			
	}	
	return NO_ERROR;	
}
int8_t read_information(flash_t *flash)
{
	uint8_t check_result;
	uint8_t count  = 0;
	uint8_t len = 0;
	flash->status = (S25FL1_read_id_flash(flash) < 0)?0:1;
	for(count = 0; count < 8; count ++)
	{
		ConvertUINT8ToHEXCHAR((uint8_t *)&uid_flash_arr[len + count * 2],flash->uid_flash[count]);
	}
}