#include "main.h"
//#include "data_base.h"
#include "stm32f0xx_hal.h"
#include "Time.h"
#include "rs485.h"
#include "do.h"
#include "bootloader.h"
#include "stm32f0xx_hal_tim.h"
#include "Test.h"

int8_t test_init(void)
{
	int8_t check_init_flash = 0;
	uint8_t count_start_false = 0;
	/* Khoi tao debug */
	uart_debug_init();
	/* Khoi tao watchdog */
	watchdog_ex_initialize();
	/* Khoi tao timer cho watchdog */
	timer16_init();
	/* Khoi tao flash */
	do
	{
		check_init_flash =  config_init();
		count_start_false ++;
	}while(count_start_false < NUMBER_TRY_INIT && check_init_flash < 0);
	if(check_init_flash < 0)
	{
		debug_len = sprintf((char *)&debug_arr[0],"Init flash false! \n");
		debug_send(&debug_arr[0],debug_len);		
	}else{
		/* to do */
		debug_len = sprintf((char *)&debug_arr[0],"Init flash ok! \n");
		debug_send(&debug_arr[0],debug_len);	
	}	
	/* tra ve gia tri */
	return check_init_flash;
}
int8_t test_read_write_flash(void)
{
	uint8_t data_write[65] = "This is test programer for external flash read and write succes!";
	uint8_t data_read[65];
	uint8_t count = 0;
	int16_t check_result = 0;
	uint16_t size_update = 1000;
	uint16_t size_read = 0;
	uint8_t update_request = 0;
	/* test xoa flash */
	for(count = 0; count < 20; count ++)
	{
		/* xoa sach buffer doc lai */
		memset(data_read,0,65);
		debug_len = sprintf((char *)&debug_arr[0],"test xoa flash, sector %d  \n", SECTOR_START_APP_STORE_EXT + count);
		debug_send(&debug_arr[0],debug_len);
		check_result = S25FL1_Erase_Sector(SECTOR_START_APP_STORE_EXT + count);
		if(check_result < 0 ){
			debug_len = sprintf((char *)&debug_arr[0],"eraser sector %d false\n", SECTOR_START_APP_STORE_EXT + count);
			debug_send(&debug_arr[0],debug_len);	
			return -1;
		}else{
			HAL_Delay(3); /* delay 3ms */
			/* doc lai kiem tra ket qua sector xoa chua */
			check_result = S25FL1_Read_Multi_Byte(&data_read[0],(SECTOR_START_APP_STORE_EXT + count) * SECTOR_WIDTH, SIZE_OF_PACKAGE);
			if(check_result < 0 ){
				debug_len	= sprintf((char *)&debug_arr,"read after eareser sector %d false!\n",SECTOR_START_APP_STORE_EXT + count);
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				return -1;
			}else{
				for(uint8_t i = 0; i <  SIZE_OF_PACKAGE; i ++)
				{
					if(data_read[i] != 0xff)
					{
						debug_len	= sprintf((char *)&debug_arr,"verify after eareser sector %d false !\n",SECTOR_START_APP_STORE_EXT + count);
						debug_send((uint8_t *)&debug_arr[0],debug_len);
						return -1;
					}else{
						/* to do */
					}		
				}
				debug_len	= sprintf((char *)&debug_arr,"xoa sector %d success !\n",SECTOR_START_APP_STORE_EXT + count);
				debug_send((uint8_t *)&debug_arr[0],debug_len);				
			}
		}
	}
	/* test doc va ghi sector */
	for(count = 0; count < 20; count ++)
	{
		/* xoa sach buffer doc lai */
		memset(data_read,0,65);
		debug_len = sprintf((char *)&debug_arr[0],"test xoa read, write sector %d  \n", SECTOR_START_APP_STORE_EXT + count);
		debug_send(&debug_arr[0],debug_len);
		debug_len = sprintf((char *)&debug_arr[0],"\nWrite: \n");
		debug_send(&debug_arr[0],debug_len);	
		debug_send(&data_write[0],SIZE_OF_PACKAGE);	
		
		check_result = S25FL1_Write_Multi_Byte(&data_write[0],(SECTOR_START_APP_STORE_EXT + count) * SECTOR_WIDTH, SIZE_OF_PACKAGE);
		if(check_result < 0 ){
			debug_len = sprintf((char *)&debug_arr[0],"write sector %d false!\n", SECTOR_START_APP_STORE_EXT + count);
			debug_send(&debug_arr[0],debug_len);	
			return -1;
		}else{
			HAL_Delay(3); /* delay 3ms */
			/* doc lai kiem tra ket qua sector xoa chua */
			check_result = S25FL1_Read_Multi_Byte(&data_read[0],(SECTOR_START_APP_STORE_EXT + count) * SECTOR_WIDTH, SIZE_OF_PACKAGE);
			debug_len = sprintf((char *)&debug_arr[0],"\nRead: \n");
			debug_send(&debug_arr[0],debug_len);
			debug_send(&data_read[0],SIZE_OF_PACKAGE);
			
			if(check_result < 0 ){
				debug_len	= sprintf((char *)&debug_arr,"read after write sector %d false!\n",SECTOR_START_APP_STORE_EXT + count);
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				return -1;
			}else{
				for(uint8_t i = 0; i <  SIZE_OF_PACKAGE; i ++)
				{
					if(data_read[i] != data_write[i])
					{
						debug_len	= sprintf((char *)&debug_arr,"verify after write sector %d false !\n",SECTOR_START_APP_STORE_EXT + count);
						debug_send((uint8_t *)&debug_arr[0],debug_len);
						return -1;
					}else{
						/* to do */
					}		
				}
			debug_len	= sprintf((char *)&debug_arr,"read and write sector %d success !\n",SECTOR_START_APP_STORE_EXT + count);
			debug_send((uint8_t *)&debug_arr[0],debug_len);				
			}
		}
	}
	/* test update size */
	check_result = update_size(size_update);
	if(check_result < 0)
	{
		debug_len	= sprintf((char *)&debug_arr,"Update size false !\n");
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		return -1;
	}else{
		/* doc lai kich thuoc */
		size_read = get_size();
		debug_len	= sprintf((char *)&debug_arr,"sizeof read %d !\n",size_read);
		debug_send((uint8_t *)&debug_arr[0],debug_len);
	}
	/* test request update */
	check_result = request_update_firmware();
	if(check_result < 0)
	{
		debug_len	= sprintf((char *)&debug_arr,"Request update false !\n");
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		return -1;
	}else{
		/* doc lai yeu cau */
		update_request = check_update();
		if(update_request == New_Update)
		{
			debug_len	= sprintf((char *)&debug_arr,"request update pass !\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
		}else{
			debug_len	= sprintf((char *)&debug_arr,"request update false !\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);	
			return -1;
		}
	}
	/* test xoa update */
	check_result = clear_request_update_firmware();
	if(check_result < 0)
	{
		debug_len	= sprintf((char *)&debug_arr,"Clear update false !\n");
		debug_send((uint8_t *)&debug_arr[0],debug_len);
		return -1;
	}else{
		/* doc lai yeu cau */
		update_request = check_update();
		if(update_request == No_Update)
		{
			debug_len	= sprintf((char *)&debug_arr,"Clear update pass !\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
		}else{
			debug_len	= sprintf((char *)&debug_arr,"Clear update false !\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);	
			return -1;
		}
	}	
	return 0;
}
int8_t test_function(void)
{
	test_read_write_flash();
	return 0;
}