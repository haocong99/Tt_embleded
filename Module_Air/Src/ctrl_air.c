#include "do.h"
#include "time.h"
#include "stm32f0xx_hal_gpio.h"
#include "rs485.h"
#include "data_base.h"
#include "temperature.h"
#include "ctrl_air.h"

static uint8_t check_request = 0;
static int16_t temp_air_door_max = 0;
static int16_t temp_air_door_min = 0;
static 	uint32_t number_check = 0;
static uint8_t status_air = 0x01;
uint8_t error_air_module = 0;
uint8_t air_id =  SHARP_AIR;
/* khai bao cmd to get data */
cmd_ctrl_air_t air_cmd;
/* air on at 23C and cool mode */
cmd_ctrl_air_t table_control[NUMBER_CMD_CTRL] ={
								                {SHARP_AIR_OFF,{0xAA,0x5A,0xCF,0x10,0x7C,0x21,0x72,0x00,0x08,0x80,0x00,0xE0,0xF1},SHARP_CMD_LEN},	\
												{SHARP_AIR_ON,{0xAA,0x5A,0xCF,0x10,0xC8,0x11,0x22,0x00,0x08,0x80,0x00,0xE0,0x61},SHARP_CMD_LEN},	\
												{DAIKIN_ARC433A46_OFF,{0x11,0xDA,0x27,0x00,0xC5,0x00,0x00,0xD7,0x11,0xDA,0x27,0x00,0x42,0x00,0x00,0x54,0x11,0xDA,0x27,0x00,0x00,0x08,0x2E,0x00,0xA0,0x00,0x00,0x06,0x60,0x00,0x00,0xC0,0x00,0x00,0x0E},DAIKIN_ARC433A46_CMD_LEN},	\
												{DAIKIN_ARC433A46_ON, {0x11,0xDA,0x27,0x00,0xC5,0x00,0x00,0xD7,0x11,0xDA,0x27,0x00,0x42,0x04,0x00,0x58,0x11,0xDA,0x27,0x00,0x00,0x39,0x2E,0x00,0xA0,0x00,0x00,0x06,0x60,0x00,0x00,0xC0,0x00,0x00,0x3F},DAIKIN_ARC433A46_CMD_LEN},	\
												{DAIKIN_ARC423A5_OFF,{0x11,0xDA,0x27,0xF0,0x0D,0x00,0x0F,0x11,0xDA,0x27,0x00,0xD3,0x30,0xF1,0x00,0x00,0x1A,0x03,0x08,0x2B},DAIKIN_ARC423A5_CMD_LEN},	\
												{DAIKIN_ARC423A5_ON, {0x11,0xDA,0x27,0xF0,0x0D,0x00,0x0F,0x11,0xDA,0x27,0x00,0xD3,0x31,0x41,0x00,0x00,0x1A,0x07,0x08,0x80},DAIKIN_ARC423A5_CMD_LEN}, // 23C, fan max, timer off, swing off goc 72
												{REETECH_OFF,{0x4D,0xB2,0xDE,0x21,0x07,0xF8},REETECH_CMD_LEN},	\
												{REETECH_ON, {0x4D,0xB2,0xF8,0x07,0x1C,0xE3},REETECH_CMD_LEN} /* 19C on, fan off, swing off, auto mode */
};
#define BYTE_TEMP_REETECH	4
#define TEMP_INIT_DEFAULT_REETECH	17 /* thap nhat 17 C*/
const uint8_t temp_reetech[15] ={0x10,0x18,0x1C,0x014,0x16,0x1E,0x1A,0x12,0x13,0x1B,0x19,0x11,0x15,0x1D};
void set_request_check_air(void)
{
	check_request = 1;
}
void clear_request_check_air(void)
{
	check_request = 0;
}
uint8_t get_request_check_air(void)
{
	return check_request;
}

void set_temp(uint8_t temp, uint8_t id_air)
{
	uint8_t buf = 0;
	uint32_t crc_recal = 0;
	switch(id_air)
	{
		case SHARP_AIR: /* dieu hoa sharp */
			#if 0
			buf = 0xC0 + (temp -15);
			table_control[0].cmd_content[4] = buf;
			#endif
			/* Tinh lai crc */
		break;
		case DAIKIN_AIR_ARC433A46: /* dieu hoa sharp */
			buf = table_control[DAIKIN_ARC433A46_ON].cmd_content[22] = temp << 1;
			/* tinh lai crc */
			for(uint8_t count  = 16; count < 34; count ++)
				crc_recal += table_control[DAIKIN_ARC433A46_ON].cmd_content[count];
				table_control[DAIKIN_ARC433A46_ON].cmd_content[DAIKIN_ARC433A46_CMD_LEN - 1] = (uint8_t)(crc_recal & 0xff);	
		break;
		case DAIKIN_AIR_ARC423A5: 
			buf = table_control[DAIKIN_ARC423A5_ON].cmd_content[16] = 0x10 + (temp - 18) * 2;
			/* tinh lai crc */
			for(uint8_t count  = 7; count < 19; count ++)
				crc_recal += table_control[DAIKIN_ARC423A5_ON].cmd_content[count];
				table_control[DAIKIN_ARC423A5_ON].cmd_content[DAIKIN_ARC423A5_CMD_LEN - 1] = (uint8_t)(crc_recal & 0xff);	
		break;
		case REETECH_AIR:
			buf = temp_reetech[temp - TEMP_INIT_DEFAULT_REETECH];
			table_control[REETECH_ON].cmd_content[BYTE_TEMP_REETECH] = buf;
			/* tinh lai CRC */
			table_control[REETECH_ON].cmd_content[BYTE_TEMP_REETECH + 1] = ~buf; /* dao byte nhiet do*/
			break;
	}
}
void set_mode(uint8_t mode, uint8_t id_air)
{
	uint8_t buf = 0;
	uint32_t crc_recal = 0;
	switch(id_air)
	{
		case SHARP_AIR: /* dieu hoa sharp */
			#if 0
			if(mode == COOL_MODE)
				table_control[SHARP_AIR_ON].cmd_content[6] = 0x22;
			else if(mode == DRY_MODE)
				table_control[SHARP_AIR_ON].cmd_content[6] = 0x23;
			else /* auto mode */
				table_control[SHARP_AIR_ON].cmd_content[6] = 0x20;
			#endif
			/* Tinh lai crc */
		break;
		case DAIKIN_AIR_ARC433A46: /* dieu hoa sharp */
			buf = table_control[DAIKIN_ARC433A46_ON].cmd_content[21];
			buf &= 0xf1;
			if(mode == COOL_MODE)
			{
				buf |= 0x04;
				table_control[DAIKIN_ARC433A46_ON].cmd_content[21] = buf;
			}
			else if(mode == DRY_MODE)
			{
				buf |= 0x02;
				table_control[DAIKIN_ARC433A46_ON].cmd_content[21] = buf;	
			}
			else /* auto mode */
			{
				buf |= 0x00;
				table_control[DAIKIN_ARC433A46_ON].cmd_content[21] = buf;	
			}
			/* tinh lai crc */
			for(uint8_t count  = 16; count < 34; count ++)
				crc_recal += table_control[DAIKIN_ARC433A46_ON].cmd_content[count];
				table_control[DAIKIN_ARC433A46_ON].cmd_content[DAIKIN_ARC433A46_CMD_LEN - 1] = (uint8_t)(crc_recal & 0xff);	
		break;
		case DAIKIN_AIR_ARC423A5: 
			if(mode == COOL_MODE)
			{
				buf |= 0x31;
				table_control[DAIKIN_ARC433A46_ON].cmd_content[12] = buf;
			}
			else if(mode == DRY_MODE)
			{
				buf |= 0x21;
				table_control[DAIKIN_ARC433A46_ON].cmd_content[12] = buf;	
			}
			/* tinh lai crc */
			for(uint8_t count  = 7; count < 19; count ++)
				crc_recal += table_control[DAIKIN_ARC423A5_ON].cmd_content[count];
				table_control[DAIKIN_ARC423A5_ON].cmd_content[DAIKIN_ARC423A5_CMD_LEN - 1] = (uint8_t)(crc_recal & 0xff);	
		break;
		case REETECH_AIR:
			/* to do */
			break;
	}
}
uint8_t check_air_operation(int16_t temp_air_door)
{
	/* if air on then check air operation */
	if(get_request_check_air())
	{
		if(temp_air_door > temp_air_door_max)
		{
			temp_air_door_max = temp_air_door;
		}
		else if(temp_air_door < temp_air_door_min)
		{
			temp_air_door_min = temp_air_door;		
		}
		else
		{
			/* to do */
		}
		number_check ++;
		/* compare max and min of temp */
		if((temp_air_door_max - temp_air_door_min) >= 30)
		{
			/* the air is oke ! */
			clear_request_check_air();
			error_air_module &= NO_ERROR_AIR;
			number_check = 0; /* clear number check */
		}
		else
		{
			if(number_check >= TIME_CHECK_TEMPERATURE)
			{
				/* request error air*/
				error_air_module |= ERROR_AIR_OPERATION;
				/* bo qua viec tiep tuc check */
				/* the air is broken ! */
				clear_request_check_air();				
			}
			else
			{
				/* to do */
			}
		}
	}
	else
	{
		/* to do */
		return 0;
	}
	return 0;
}
/**
  * @brief  This function return status on/off of two Aircondition. 
  * @param  None
  * @retval uint8_t result
  */
uint8_t do_getvalue(void)
{
	return status_air;
}

void start_condition_sharp(void)
{
	AIR_HI();
	/* tao ra 142 xung */
	create_pulse(142);
	AIR_LO();
	/* delay_us 1900 us */
	delay_us(1900);
}	
void stop_condition_sharp(void)
{
	AIR_HI();
	/* tao ra 142 xung */
	create_pulse(15);
	AIR_LO();
	/* delay_us 1900 us */
	delay_us(3084);
	AIR_HI();
	delay_us(10);
	AIR_LO();
}
void start_condition_reetech(void)
{
	AIR_HI();
	/* tao ra 168 xung */
	create_pulse(168);
	AIR_LO();
	/* delay_us 4300 us */
	delay_us(4300);
}
void stop_condition_reetech(void)
{
	/* phat ra khoang 15 xung */
	create_pulse(17);
	/* ket thuc bit luon la 1 de chuan bi xung tiep theo*/
	AIR_LO();
	/* sau do delay_us mot hoang 492us */
	delay_us(5300);
}
void start_condition_daikin(uint8_t daikin_id)
{
	uint8_t i = 0;
	switch(daikin_id)
	{
		case DAIKIN_AIR_ARC433A46:
			/* tao ra 5 bit 0 */
			for(i =0; i <6 ; i++)
			bit_0();
			AIR_LO();
			/* delay_us mot khoang */
			delay_us(29150);	/* tao ra 130 xung */
			create_pulse(130);
			bit_1();
		break;
		case DAIKIN_AIR_ARC423A5:
			AIR_LO();
			delay_us(25450);
			AIR_HI();	
			//delay_us(4950);
			create_pulse(188);
			AIR_LO();
			/* delay_us mot khoang */
			delay_us(2150);	/* tao ra 130 xung */
			AIR_HI();	
			//delay_us(300);	/* tao ra 130 xung */
			create_pulse(11);
		break;
		default:
		break;
	}
	return;
}

void restart_condition_daikin(uint8_t daikin_id)
{
	switch(daikin_id)
	{
		case DAIKIN_AIR_ARC433A46:
			bit_1();
			AIR_LO();
			/* delay_us mot khoang */
			delay_us(34910);
			/* tao ra 130 xung */
			create_pulse(130);
			bit_1();
		break;
		case DAIKIN_AIR_ARC423A5:
			bit_1();
			AIR_LO();
			delay_us(25450);
			AIR_HI();	
			//delay_us(4950);
			create_pulse(188);
			AIR_LO();
			/* delay_us mot khoang */
			delay_us(2150);	/* tao ra 130 xung */
			AIR_HI();	
			//delay_us(300);	/* tao ra 130 xung */
			create_pulse(11);
		break;
		default:
		break;
	}
	return;	
}
void stop_condition_daikin(uint8_t daikin_id)
{
	switch(daikin_id)
	{
		case DAIKIN_AIR_ARC433A46:
			bit_1();
			AIR_LO();
			/* delay_us 1900 us */
			delay_us(50000);
		break;
		case DAIKIN_AIR_ARC423A5:
			bit_1();
			AIR_LO();
			delay_us(50000);
		break;
		default:
		break;
	}
	return;
}

void control_air_sharp(uint8_t cmd_air)
{
	uint8_t count = 0;
	uint8_t len = 0;
	/* xoa sach buffer truoc khi su dung */
	memset((uint8_t *)&air_cmd.cmd_content[0],0,MAX_LEN_CMD);
	/* xoa chi so do dai cau lenh */
	air_cmd.len_cmd = 0;
	/* get cau lenh trong bang lenh */
	if(cmd_air)
	{
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[SHARP_AIR_ON].cmd_content[0],table_control[SHARP_AIR_ON].len_cmd);
	}
	else
	{
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[SHARP_AIR_ON].cmd_content[0],table_control[SHARP_AIR_ON].len_cmd);
	}
	air_cmd.len_cmd = SHARP_CMD_LEN; 
	/* Gui cau lenh */
	for(count = 0; count < NUMBER_REPEAT_SEND_CMD; count ++)
	{
		/* Gui tin hieu start */
		start_condition_sharp();
		/* Gui du lieu */
		for(len = 0; len < air_cmd.len_cmd; len ++)
			sendbyte_data(air_cmd.cmd_content[len]);
		/* Gui stop */
		stop_condition_sharp();
		WATCHDOG_REFRESH();
	}
}
void control_air_daikin(uint8_t cmd_air, uint8_t daikin_id)
{
	uint8_t count = 0;
	uint8_t len = 0;
	/* xoa sach buffer truoc khi su dung */
	memset((uint8_t *)&air_cmd.cmd_content[0],0,MAX_LEN_CMD);
	/* xoa chi so do dai cau lenh */
	air_cmd.len_cmd = 0;
	switch(daikin_id)
	{
		case DAIKIN_AIR_ARC433A46:
			/* Lay du lieu cau lenh */
			if(cmd_air)
			{
				memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[DAIKIN_ARC433A46_ON].cmd_content[0],table_control[DAIKIN_ARC433A46_ON].len_cmd);
			}
			else
			{
				memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[DAIKIN_ARC433A46_OFF].cmd_content[0],table_control[DAIKIN_ARC433A46_OFF].len_cmd);
			}
			air_cmd.len_cmd = DAIKIN_ARC433A46_CMD_LEN;
			/* Gui cau lenh */
			for(count = 0; count < NUMBER_REPEAT_SEND_CMD; count ++)
			{
				/* start condition */
				start_condition_daikin(DAIKIN_AIR_ARC433A46);
				/* send data 8 byte dau */
				for(len = 0; len < 8; len ++)
					sendbyte_data(air_cmd.cmd_content[len]);
				/* restart */
				restart_condition_daikin(DAIKIN_AIR_ARC433A46);
				/* send next 8 byte tiep theo */
				for(len = 8; len < 16; len ++)
					sendbyte_data(air_cmd.cmd_content[len]);	
				/* restart */
				restart_condition_daikin(DAIKIN_AIR_ARC433A46);
				/* send next 8 byte tiep theo */
				for(len = 16; len < air_cmd.len_cmd; len ++)
					sendbyte_data(air_cmd.cmd_content[len]);	
				/* stop condition */
				stop_condition_daikin(DAIKIN_AIR_ARC433A46);
				WATCHDOG_REFRESH();
			}
			break;
		case DAIKIN_AIR_ARC423A5:
			/* Lay du lieu cau lenh */
			if(cmd_air)
			{
				memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[DAIKIN_ARC423A5_ON].cmd_content[0],table_control[DAIKIN_ARC423A5_ON].len_cmd);
			}
			else
			{
				memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[DAIKIN_ARC423A5_OFF].cmd_content[0],table_control[DAIKIN_ARC423A5_OFF].len_cmd);
			}
			air_cmd.len_cmd = DAIKIN_ARC423A5_CMD_LEN; 
			/* Gui cau lenh */
			for(count = 0; count < NUMBER_REPEAT_SEND_CMD; count ++)
			{
				/* start condition */
				start_condition_daikin(DAIKIN_AIR_ARC423A5);
				/* send data 7 byte dau */
				for(len = 0; len < 7; len ++)
					sendbyte_data(air_cmd.cmd_content[len]);
				/* restart */
				restart_condition_daikin(DAIKIN_AIR_ARC423A5);
				/* send next 13 byte tiep theo */
				for(len = 7; len < air_cmd.len_cmd; len ++)
					sendbyte_data(air_cmd.cmd_content[len]);		
				/* stop condition */
				stop_condition_daikin(DAIKIN_AIR_ARC423A5);
				WATCHDOG_REFRESH();
			}
			break;
		default:
			break;
	}
}
void control_air_reetech(uint8_t cmd_air)
{
	uint8_t count = 0;
	uint8_t len = 0;
	/* xoa sach buffer truoc khi su dung */
	memset((uint8_t *)&air_cmd.cmd_content[0],0,MAX_LEN_CMD);
	/* xoa chi so do dai cau lenh */
	air_cmd.len_cmd = 0;
	/* get cau lenh trong bang lenh */
	if(cmd_air)
	{
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[REETECH_ON].cmd_content[0],table_control[REETECH_ON].len_cmd);
	}
	else
	{
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[REETECH_OFF].cmd_content[0],table_control[REETECH_OFF].len_cmd);
	}
	air_cmd.len_cmd = REETECH_CMD_LEN; 
	/* Gui cau lenh */
	for(count = 0; count < NUMBER_REPEAT_SEND_CMD; count ++)
	{
		/* Gui tin hieu start */
		start_condition_reetech();
		/* Gui du lieu */
		for(len = 0; len < air_cmd.len_cmd; len ++)
			sendbyte_data(air_cmd.cmd_content[len]);
		/* Gui stop */
		stop_condition_reetech();
		WATCHDOG_REFRESH();
	}
}
void control_air(uint8_t cmd, uint8_t air_id)
{
	/* thuc hien viec dieu khien */
	switch(air_id)
	{
		case SHARP_AIR:
			control_air_sharp(cmd);
			break;
		case REETECH_AIR:
			control_air_reetech(cmd);
		break;
		case DAIKIN_AIR_ARC423A5:
			control_air_daikin(cmd, DAIKIN_AIR_ARC423A5);
			break;
		case DAIKIN_AIR_ARC433A46:
			control_air_daikin(cmd, DAIKIN_AIR_ARC433A46);
			break;
		default:
			break;
	}
	/* sau khi dieu khien gan lai trang thai cua cac dieu hoa */
	status_air = cmd;	
}
