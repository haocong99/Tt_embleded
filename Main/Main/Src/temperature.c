#include "main.h"
#include "do.h"
#include "stm32f0xx_hal.h"
#include "temperature.h"
#include "Time.h"
#include "crc.h"
#include "rs485.h"
#include "ctrl_air.h"
#include "system_start.h"
int32_t temp_read = 0;
/*
 * @brief: This function rearange temperature and humidity
 * @param: none
 * @return: none
 */
int16_t filter_temp(int16_t *temp_buf, uint8_t numbers)
{
	int16_t tg = 0;
	uint8_t i;
	uint8_t j;
	int16_t buf[numbers];
	/* coppy mang */
	for(i = 0; i < numbers; i++)
	{
		buf[i] = temp_buf[i];
	}
	/* sap xep theo thu tu tang dan */
	for(i = 0; i < numbers - 1; i ++)
	{
		for(j = i+1; j < numbers ; j++ )
			{
				if(buf[i] > buf[j])
				{
					tg = buf[j];
					buf[j] = buf[i];
					buf[i] = tg;
				}
			}
	}
	/* neu so luong la le */
	if(numbers % 2)
	{
		return buf[numbers % 2];
	}
	/* neu so luong la chan */
	return buf[numbers % 2 - 1];
}
void sensor_pin_input(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    __GPIOB_CLK_ENABLE();
	
	/* initialize air 1*/
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = TEM_SS1_PIN  ;
	HAL_GPIO_Init(TEM_SS1_PORT , &GPIO_InitStructure);
}
void sensor_pin_output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    __GPIOB_CLK_ENABLE();
	
	/* initialize air 1*/
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = TEM_SS1_PIN  ;
	HAL_GPIO_Init(TEM_SS1_PORT , &GPIO_InitStructure);
}
int16_t pulse_reset(void)
{
	uint8_t retry  = 125;
	uint8_t r = 0;
	
	TEM_SS_PIN_HIGH();
	/* set output pin */
	sensor_pin_output();
	Delay(100);
	/* sau do chuyen sang input pin */
	sensor_pin_input();
	// wait until the wire is high... just in case
	do {
		if (--retry == 0) return -1;
		Delay(2000); /* 2ms */
	} while ( !INPUT_SS_BIT());	
	/* set pin low */
	TEM_SS_PIN_LOW();
	/* set output pin */
	sensor_pin_output();
	/* delay 480 us */
	Delay(1000);
	TEM_SS_PIN_HIGH();
	/* set input */
	sensor_pin_input();
	/* wait 70 us */
	Delay(100);
	/* read pin */
	r = INPUT_SS_BIT();
	/* wait 300 us */
	Delay(500);
	return r;
}
void ss_send_bit1(void)
{
	/* keo ss ve 0 trong 10 us */
	TEM_SS_PIN_LOW();
	/* set output pin */
	sensor_pin_output();
	/* giu trong 10 us */
	Delay(5);
	/* keo ss len 1 trong 60 us */
	TEM_SS_PIN_HIGH();
	/* giu trong 60 us */
	Delay(70);
}
void ss_send_bit0(void)
{
	/* keo ss ve 0 trong 10 us */
	TEM_SS_PIN_LOW();
	/* set output pin */
	sensor_pin_output();
	/* giu trong 10 us */
	Delay(70);
	/* keo ss len 1 trong 60 us */
	TEM_SS_PIN_HIGH();
	/* giu trong 10 us */
	Delay(5);
}
uint8_t read_bit(void)
{
	uint8_t r = 0;
	uint32_t time_wait_pin_high = 0;
	/* keo ss ve 0 trong 10 us */
	TEM_SS_PIN_LOW();
	/* set output pin */
	sensor_pin_output();
	/* giu trong 10 us */
    Delay(1);
	TEM_SS_PIN_HIGH();
	/* set output pin */
	sensor_pin_input();
	time_wait_pin_high = wait_state(GPIO_PIN_SET, 100);
	r = (time_wait_pin_high > 6)?0:1;
	Delay(50);
//	Delay(4);
//	r = INPUT_SS_BIT();
//	Delay(50);
	return r;
}
void send_byte(uint8_t dat)
{
	uint8_t i  = 0;
	for(i = 0; i < 8; i++)
	{
		/* send low bit first */
		((dat >> i) & 0x01)?ss_send_bit1(): ss_send_bit0();
	}
}
uint8_t read_byte(void)
{
	uint8_t i = 0;
	uint8_t data = 0;
	/* the least bit first */
	for(i = 0; i < 8; i++)
	{
		data |= (read_bit() << i);
	}
	return data;
}
int16_t sensor_temp_init(void)
{

	/* init io */
	int16_t result  = -1;
	uint8_t count  = 0;
	sensor_pin_output();
	do{
		result = pulse_reset();
		count ++;
	} while((result != 0) && (count < 3));
	if(result  != 0)
	{
		return -1;
	}
	send_byte(SKIP_ROM_CMD);
	/* Write Scratchpad */
	send_byte(WRITE_SCRATCHPAD_CMD);
	/* MASTER Tx TL BYTE TO SCRATCHPAD*/
	send_byte(TL_MIN);
	/* MASTER Tx TH BYTE TO SCRATCHPAD*/
	send_byte(TH_MAX);
	/* MASTER TX CONFIG. BYTE TO SCRATCHPAD */
	send_byte(RESOLUTION_11B);
	return 0;
}
int16_t sensor_get_data(int16_t *temp)
{
	 int16_t temperature = 0;
	/* init io */
	int16_t result  = -1;
	uint8_t count  = 0;
	uint8_t crc = 0;
	uint8_t arr[9] = {0};
	do{
		result = pulse_reset();
		count ++;
	} while((result != 0) && (count < 3));
	if(result  != 0)
	{
		return -1;
	}
	send_byte(SKIP_ROM_CMD);
	/* 44h CONVERT */
	send_byte(CONVERT_TEMP_CMD);
	/* wait util convert done */
	/* wait DS18B20 CONVERSION  by checking signal */
	do{
		count ++;
		Delay(5000); /* 5 ms */
	}while((read_bit() != GPIO_PIN_SET) && (count < 100)); /* not excceed 200us */
	if(count >= 100)
	{
		return -2;
	}
	/* doc ve nhiet do */
	count  = 0;
	do{
		result = pulse_reset();
		count ++;
	} while((result != 0) && (count < 3));
	if(result  != 0)
	{
		return -3;
	}
	/* send skip rom cmd */
	send_byte(SKIP_ROM_CMD);
	/* BEh READ SCRATCHPAD */
	send_byte(READ_TEMP_CMD);
	/* read 9 byte */
	for(count = 0; count < 9; count ++)
	{
		arr[count] = read_byte(); 
	}
#if 0	
	/* check du lieu */
	if(arr[7] != 0x10)
	{
		return -4;
	}
	/* check du lieu sensor ngay 28/07/2018*/
	if(arr[7] != 0x66)
	{
		return -4;
	}	
#endif	
	/* crc calculate */
	crc = Cal_CRC8(&arr[0], 8);
	if(crc != arr[8])
	{
//		debug_len = sprintf((char *)&debug_arr[0],"CRC_READ/CAL:%d,%d!\r\n",arr[8],crc);
//		debug_send(&debug_arr[0],debug_len);
		 return -5;
	}
	/* calculate temperature */
	temperature = arr[0]|(arr[1] << 8);
	if(arr[1] & 0x8000) /* nhiet do am */
		temperature = ~temperature +1; /* ma bu 2 */
	temperature >>=3; /* dich xuong 1 bit */
	temperature = ((float)temperature) /2.0 * 10; /* convert ra nhiet do that, nhan len 10 lan */
	if(temperature == 850 || temperature == 0) /* truong hop loi */
	{
		/* truoc mat bo qua da */
		result  = -6;
		return result;
	}
	/* attach temperature */
	*temp =  temperature;
	return 0;
}
int16_t tempearture_read(void)
{
	static uint8_t count_read_false = 0;
	static uint8_t count_read_ok = 0;
	static int16_t buffer_temp[5];
//	uint8_t val_inv = 0;
	int16_t buf;
	int32_t result = sensor_get_data(&buf);
	if( result < 0)
	{
		count_read_ok = 0;
		/* read false */
		count_read_false++;
		if(count_read_false > NUMBER_READ_FALSE) /* false excced 60 times */
		{
			count_read_false = 0;
			/* Set canh bao sensor hong */
			error_air_module |= ERROR_SENSOR_TEMP;
			/* Sensor bi loi bo qua viec check dieu hoa loi thong qua nhiet do vi luc nay nhiet do khong con chinh xac */
			clear_request_check_air();
		}
		else
		{
			/* to do */
		}
		return -1;
	}
	else
	{
		/* read ok */
		if(count_read_ok > NUMBER_READ_OK)
		{
			/* sap sep */
			count_read_false = 0;
			buffer_temp[0] = buffer_temp[1];
			buffer_temp[1] = buffer_temp[2];
			buffer_temp[2] = buf;
			temp_read = filter_temp(&buffer_temp[0], 3);
		}
		else
		{
			buffer_temp[count_read_ok] = buf;
			temp_read = buf;
			count_read_ok ++;
			
		}
		error_air_module &= NO_ERROR_SENSOR;
		/* Neu cam bien nhiet do da doc duoc thi phai xem, neu dieu hoa dang bat 
			va khong co yeu cau check van hanh phai bat lai.
		*/
		/* Day la tinh huong cam bien nhiet do bi hong tu dong phuc hoi hoac 
			duoc thay moi, phai yeu cau check lai van hanh.
		*/
		if((do_getvalue() == 1) && (get_request_check_air() == 0))
		{
				set_request_check_air();
		}else{
			/* to do*/
		}
	}
	/* check air operation */
	check_air_operation(temp_read);
	return 0;
}
int32_t get_temperature(void)
{
	return temp_read;
}
int32_t get_error(void)
{
	return error_air_module;
}
uint32_t wait_state(uint8_t state, uint32_t timeout)
{
	uint32_t count_time = 0;
	uint8_t state_pin = 0;
	do{
		state_pin = INPUT_SS_BIT();
		count_time ++;
	}while(state_pin != state && count_time < timeout);
	return count_time;
}
