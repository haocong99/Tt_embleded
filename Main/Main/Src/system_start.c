#include "system_start.h"
#include "ctrl_air.h"
static uint8_t system_error = NO_ERROR;
udi_t uid_mcu;
uint8_t hardware_version[32] = "AMCS-C4.HW.V1.0.3";
uint8_t software_version[32] = "T6_H01_F50_R01";
void readout_protection_code(void)
{
//	char debug[100];
//	uint8_t len  = 0;
	/* Thuc hien lock code chong doc nguoc code */
	FLASH_OBProgramInitTypeDef OBProgramInit;
	/* Doc cau hinh hien tai */
	/* Allow Access to option bytes sector */ 	
	HAL_FLASHEx_OBGetConfig(&OBProgramInit);
	if(OBProgramInit.RDPLevel != OB_RDP_LEVEL_1)
	{
		/* Type : restric read out */
		OBProgramInit.OptionType = OPTIONBYTE_RDP; 
		/* State setup */
		//OBProgramInit.WRPState = OB_WRPSTATE_DISABLE;
		/* Write protection page */
		//OBProgramInit.WRPPage = OB_WRP_ALLPAGES;
		/* Level */
		OBProgramInit.RDPLevel = OB_RDP_LEVEL_1;
		/* User config */
		//OBProgramInit.USERConfig = OB_STOP_NO_RST;
		/* Data adress */
		//OBProgramInit.DATAAddress = OB_DATA_ADDRESS_DATA1;
		/* Data data */
		//OBProgramInit.DATAData = 0x00;
		/* Unlock flash */
		HAL_FLASH_Unlock();
		/* Clear all FLASH flags */  
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);  		
		HAL_FLASH_OB_Unlock();	
		if(HAL_FLASHEx_OBProgram(&OBProgramInit) != HAL_OK)
		{
			/* Lock false */
		}
		FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
		HAL_FLASHEx_OBGetConfig(&OBProgramInit);
		/* Start the Option Bytes programming process */  
		if (HAL_FLASH_OB_Launch() != HAL_OK)
		{
			/* User can add here some code to deal with this error */	
		}	
	}else{
			/* User can add here some code to deal with this error */
	}
	HAL_FLASH_OB_Lock();	
}
/* Lay ra mcu id*/
void get_uid_mcu(uint32_t *uid)
{
	uid_mcu.uid_value[0] = *(uint32_t *)UID_BASE;
	uid_mcu.uid_value[1] = *(uint32_t *)(UID_BASE + 0x04);
	uid_mcu.uid_value[2] = *(uint32_t *)(UID_BASE + 0x08);
	/* Debug */
	debug_len = sprintf((char *)&debug_arr[0],"UID: ");
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	for(uint8_t count = 0; count < 12; count ++)
	{
		debug_len = sprintf((char *)&debug_arr[0],"%d ",uid_mcu.uid_byte[count]);
		debug_send((uint8_t *)&debug_arr[0],debug_len);
	}
	debug_send((uint8_t *)&"\n",1);
}
/* Ham nay test flash bang cach xoa thu mot sector, ghi vao mot chuoi, 
doc ra chuoi do, so sanh voi chuoi ghi vao */
uint8_t test_flash(void)
{
	uint8_t i = 0;
	uint8_t arr_read[6] = {0,0,0,0,0};
	/* xoa flash */	
	S25FL1_Erase_Sector(SECTOR_TEST_FLASH);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Read_Byte(&arr_read[i], ADD_TEST_FLASH +i) != READ_OK_STATUS)
		{
			debug_len = sprintf((char *)&debug_arr[0],"Doc flash sau xoa loi\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			return ERROR_FLASH;
		}else {
			if(arr_read[i] != 0xff) /* neu xoa khong duoc gia tri nay co the khac 0xff */
			{
				debug_len = sprintf((char *)&debug_arr[0],"Xoa flash loi byte[%d] = %d\n",i,arr_read[i]);
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				return ERROR_FLASH;				
			}
		}
	}
	memset(&arr_read[0],0,6);
	/* check flash */
	debug_len = sprintf((char *)&debug_arr[0],"Ghi flash : 0,1,2,3,4\n");
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Write_Byte(i,  ADD_TEST_FLASH + i) != WRITE_OK_STATUS)
		{
			debug_len = sprintf((char *)&debug_arr[0],"Ghi flash loi\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			return ERROR_FLASH;
		}
	}
	HAL_Delay(100);
	for(i = 0; i<5; i++)
	{
		if(S25FL1_Read_Byte(&arr_read[i], ADD_TEST_FLASH +i) != READ_OK_STATUS)
		{
			debug_len = sprintf((char *)&debug_arr[0],"Doc flash loi\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			return ERROR_FLASH;
		}else{
			if(i != arr_read[i])
			{
				debug_send((uint8_t *)"Khoi tao flash loi\n",19);
				return ERROR_FLASH;
			}			
		}
	}
	debug_len = sprintf((char *)&debug_arr[0],"Doc flash: %d,%d,%d,%d,%d\n",arr_read[0],arr_read[1],arr_read[2],arr_read[3],arr_read[4]);
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	return NO_ERROR;
}
/* Khoi tao va doc thu sensor */
uint8_t test_sensor(void)
{
	int16_t temp_test = 0;
	uint8_t count_test = 0;
	uint8_t error_read_sensor = ERROR_SENSOR_TEMP;
	while((count_test < NUMBER_TRY_READ) && (error_read_sensor == ERROR_SENSOR_TEMP))
	{
		if( sensor_get_data(&temp_test) == NO_ERROR)
		{
			debug_len = sprintf((char *)&debug_arr[0],"Doc nhiet do thanh cong %d C\n",temp_test);
			debug_send((uint8_t *)&debug_arr[0],debug_len);	
			HAL_Delay(20);
			error_read_sensor = NO_ERROR;
			break;
		}else{
			/* to do */
		}
		count_test ++;
	}
	return error_read_sensor;
}
uint8_t Serial_number[50];
int8_t write_uid_module(uint8_t *uid_write, uint8_t *uid_read)
{
	uint8_t i = 0;
	uint8_t crc_cal = 0;
	uint8_t crc_read = 0;
	memset(&uid_read[0],'F',24);
	/* Check flash */
	if(S25FL1_IsReady() == BUSY_STATUS){
		/* Flash timeout */
		return -1;
	}	
	crc_cal = Cal_CRC8(&uid_write[0], 24);
	/* xoa flash */	
	S25FL1_Erase_Sector(SECTOR_UID_MODULE);
	for(i = 0; i< 24; i++)
	{
		if(S25FL1_Write_Byte(uid_write[i],  ADD_UID_MODULE + i) != WRITE_OK_STATUS)
		{
			return -1;
		}
	}
	/* write crc */
	if(S25FL1_Write_Byte(crc_cal,  ADD_UID_MODULE + i) != WRITE_OK_STATUS)
	{
		return -1;
	}	
	HAL_Delay(100);
	for(i = 0; i< 24; i++)
	{
		if(S25FL1_Read_Byte(&uid_read[i], ADD_UID_MODULE +i) != READ_OK_STATUS)
		{
			return -1;
		}else{
			if(uid_read[i] != uid_write[i])
			{
				memset(&uid_read[0],'F',24);
				return -1;
			}			
		}
		
	}
	if(S25FL1_Read_Byte(&crc_read, ADD_UID_MODULE +i) != READ_OK_STATUS)
	{
		return -1;
	}else{
		crc_cal = Cal_CRC8(&uid_read[0], 24);
		if(crc_read != crc_cal)
		{
			memset(&uid_read[0],'F',24);
			return -1;
		}			
	}	
	return 0;
}
int8_t read_uid_module(uint8_t *uid_get)
{
	uint8_t i = 0;
	uint8_t crc_read = 0;
	uint8_t crc_cal = 0;
	memset(&uid_get[0],'F',24);
	/* Check flash */
	if(S25FL1_IsReady() == BUSY_STATUS){
		/* Flash timeout */
		return -1;
	}
	for(i = 0; i< 24; i++)
	{
		if(S25FL1_Read_Byte(&uid_get[i], ADD_UID_MODULE +i) != READ_OK_STATUS)
		{
			memset(&uid_get[0],'F',24);
			return -1;
		}
	}
	if(S25FL1_Read_Byte(&crc_read, ADD_UID_MODULE +i) != READ_OK_STATUS)
	{
		return -1;
	}else{
		crc_cal = Cal_CRC8(&uid_get[0], 24);
		if(crc_read != crc_cal)
		{
			memset(&uid_get[0],'F',24);
			return -1;
		}			
	}	
	return 0;	
}
uint8_t test_system(void)
{
//	uint8_t Thang = 0;
//	uint8_t Ngay = 0;
//	uint8_t Nam =0;
#if 0	
	/* Lay thang */
	if(strstr(__DATE__, "Jan"))
		Thang = 1;
	if(strstr(__DATE__, "Feb"))
		Thang = 2;
	if(strstr(__DATE__, "Mar"))
		Thang = 3;
	if(strstr(__DATE__, "Apr"))
		Thang = 4;
	if(strstr(__DATE__, "May"))
		Thang = 5;
	if(strstr(__DATE__, "Jun"))
		Thang = 6;
	if(strstr(__DATE__, "Jul"))
		Thang = 7;
	if(strstr(__DATE__, "Aug"))
		Thang = 8;
	if(strstr(__DATE__, "Sep"))
		Thang = 9;
	if(strstr(__DATE__, "Oct"))
		Thang = 10;
	if(strstr(__DATE__, "Nov"))
		Thang = 11;
	if(strstr(__DATE__, "Dec"))
		Thang = 12;
	/* Lay ngay */
	if(__DATE__[4] == ' ')
	{
		Ngay = ASCToHEXCHAR( __DATE__[5]);
	}else{
		Ngay = ASCToHEXCHAR( __DATE__[4]) *10 + ASCToHEXCHAR( __DATE__[5]);
	}
	/* Lay nam */
	Nam = ASCToHEXCHAR( __DATE__[9]) *10 + ASCToHEXCHAR( __DATE__[10]);
	/* Chuan bi du lieu */
	debug_len = sprintf((char *)&software_version[0],"C41.0%02d%02d%2d",Ngay,Thang,Nam);
#endif
    debug_len = strlen((char *)&software_version[0]);
	debug_send((uint8_t *)&software_version[0],debug_len);
	error_air_module = NO_ERROR;	
	debug_len = sprintf((char *)&debug_arr[0],"Test flash!\n");
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	error_air_module += test_flash();	
	debug_len = sprintf((char *)&debug_arr[0],"Test sensor!\n");
	debug_send((uint8_t *)&debug_arr[0],debug_len);
	error_air_module += test_sensor();
	/* Lay mcu */
	get_uid_mcu(&uid_mcu.uid_value[0]);
	/* Doc serial hien tai */
	memset(&Serial_number[0],0,50);
	read_uid_module(&Serial_number[0]);
	/* Lock code */
	//readout_protection_code();
	return system_error;
}
