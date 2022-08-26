#include "do.h"
#include "time.h"
#include "stm32f0xx_hal_gpio.h"
#include "rs485.h"
#include "data_base.h"
#include "temperature.h"
#include "sharp_air.h"
#include "system_start.h"
uint32_t last_time_ctrl = 0;
static uint8_t check_request = 0;
uint8_t status_air = 0x00; /* dieu hoa dang tat */
static 	uint32_t number_check_pass = 0;
static  uint32_t number_check_false = 0;
uint8_t error_air_module = 0;
extern IWDG_HandleTypeDef 	hwwdg;
uint8_t air_id =  SHARP_AIR;
extern uint8_t re_ctrl;
/* khai bao cmd to get data */
cmd_ctrl_air_t air_cmd;
//extern uint8_t alarm_air;
/* air on at 23C and cool mode */
const cmd_ctrl_air_t table_control[NUMBER_CMD_CTRL] ={
								        {SHARP_AIR,SHARP_AIR_OFF,{0xAA,0x5A,0xCF,0x10,0x7C,0x21,0x72,0x00,0x08,0x80,0x00,0xE0,0xF1},SHARP_CMD_LEN},	\
												{SHARP_AIR,SHARP_AIR_ON,{0xAA,0x5A,0xCF,0x10,0xC8,0x11,0x22,0x00,0x08,0x80,0x00,0xE0,0x61},SHARP_CMD_LEN}
};
//#define BYTE_TEMP_NAGAKAWA_R51M	4
//#define BYTE_TEMP_GALANZ	7
//#define BYTE_TEMP_REETECH	4
//#define TEMP_INIT_DEFAULT_REETECH	17 /* thap nhat 17 C*/
//const uint8_t temp_reetech[15] ={0x10,0x18,0x1C,0x014,0x16,0x1E,0x1A,0x12,0x13,0x1B,0x19,0x11,0x15,0x1D};
//const uint8_t temp_carrier[13] ={0x08,0x0C,0x04,0x06,0x0E,0x0A,0x02,0x03,0x0B,0x09,0x01,0x05,0x0D};
//const uint8_t temp_gree[13][2] ={{0x02,0x70},{0x03,0x80},{0x04,0x90},{0x05,0xA0},{0x06,0xB0},{0x07,0xC0},{0x08,0xD0},{0x09,0xE0},{0x0A,0xF0},{0x0B,0x00},{0x0C,0x10},{0x0D,0x20},{0x0E,0x30}};

void set_request_check_air(void)
{
	check_request = 1;
	/* Xoa cac bien dem pass va false */
	number_check_pass = 0;		
}
void clear_request_check_air(void)
{
	check_request = 0;
	/* Xoa cac bien dem pass va false */
	number_check_pass = 0;	
}
uint8_t get_request_check_air(void)
{
	return check_request;
}

void set_temp(uint8_t temp)
{
	uint8_t buf = 0;
	uint32_t crc_recal = 0;
	
	if(air_cmd.air_id == SHARP_AIR)
	{
			#if 0
			buf = 0xC0 + (temp -15);
			air_cmd.cmd_content[4] = buf;
			#endif
			/* Tinh lai crc */
	}
}
void set_mode(uint8_t mode)
{
	uint8_t buf = 0;
	uint32_t crc_recal = 0;
	if(air_cmd.air_id == SHARP_AIR)
	{
			#if 0
			if(mode == COOL_MODE)
				air_cmd.cmd_content[6] = 0x22;
			else if(mode == DRY_MODE)
				air_cmd.cmd_content[6] = 0x23;
			else /* auto mode */
				air_cmd.cmd_content[6] = 0x20;
			#endif
			/* Tinh lai crc */	
	}
}
uint8_t check_air_operation(int16_t temp_air_door)
{
	int16_t max_temp = get_temp_set_air();
//	uint32_t time_pass = 0;
	uint32_t time_now = get_time_s();
	#define TIME_TEST	900 /* Chi can 45 phut thoi */	
	max_temp *= 10; /* Nhan 10 lan nhiet do dat */
	/* Xac nhan trang thai yeu cau check */	
	if(get_request_check_air()){
		/* Xac nhan trang thai dieu hoa dang bat */
		if(do_getvalue() == 1)
		{
			/* Xac nhan dung dieu hoa bat 20C */
			if(get_temp_set_air() == 20)
			{
				/* Kiem tra nhiet do doc duoc voi nhiet do max */
				if(temp_air_door < max_temp)
				{
					/* dieu hoa binh thuong */
					number_check_pass ++;
					if(number_check_pass >= TIME_CHECK_PASS)
					{
						number_check_pass = 0;
						/* Bao dieu hoa binh thuong */
						error_air_module &= NO_ERROR_AIR;
						/* Xoas so lan false */
						number_check_false = 0;
					}else{
						/* Khong lam gi ca */
					}
				}else{
					/* Tang so lan false len, xoa ngay so lan pass */
					number_check_false ++;
					if(number_check_false >= TIME_TEST)
					{
						number_check_pass = 0;
						/* Bao dieu hoa binh thuong */
						error_air_module |= ERROR_AIR_OPERATION;
						/* Xoas so lan false */
						number_check_false = 0;
					}else{
						/* Khong lam gi ca */
					}
				}
			}else{
				number_check_false = 0;
				/* O cac muc nhiet do khac 20C xet xem dieu hoa co binh thuong khong? */
				if(max_temp >= (50 + temp_air_door))
				{
						/* Phai dam bao rang so thoi gian ma nhiet do dat moc nay la 10s lien tiep */
						if(number_check_pass >= TIME_CHECK_PASS)
						{
							/* bao dieu hoa binh thuong */
							error_air_module &= NO_ERROR_AIR;		
							/* Xoa so lan pass */
							number_check_pass = 0;
						}else{
							/* Chua du so lan thoa man dieu kien khang dinh dieu hoa binh thuong thi tang so lan len 1 */
							 number_check_pass ++;					
						}
				}else{
					/* Xoa co dem nhiet do dat  */
					number_check_pass = 0;
				}				
			}
		}else{
			/* Dang tat khong tinh */
			number_check_pass = 0;
			number_check_false = 0;
		}				
	}else{
			/* Dang tat khong tinh */
			number_check_pass = 0;
			number_check_false = 0;
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

void control_air_sharp(void)
{
	uint8_t count = 0;
	uint8_t len = 0;
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
		HAL_IWDG_Refresh(&hwwdg);
		WATCHDOG_REFRESH();
	}
}

void control_air(uint8_t cmd, uint8_t air_id)
{
	last_time_ctrl = get_time_s();
	/* Check loi truoc khi dieu khien, khi dieu hoa loi khong duoc dieu khien */
	/* truoc khi thuc hien lenh dieu khien can kiem tra dieu kien nhiet do cua gio dieu hoa */
	/* Lam sach buffer */
	memset((uint8_t *)&air_cmd.cmd_content[0],0x00,MAX_LEN_CMD);
	air_cmd.air_id = air_id;
	air_cmd.cmd = cmd;
	if(air_cmd.cmd)
	{	
		/* coppy */
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[(air_id - 1) * 2 + 1].cmd_content[0],table_control[(air_id - 1) * 2 + 1].len_cmd);
		air_cmd.len_cmd = table_control[(air_id - 1) * 2 + 1].len_cmd;
		/* set lai nhiet do cho dieu hoa truoc khi dieu khien */
		set_temp(get_temp_set_air());
		/* set mode dieu hoa truoc khi dieu khien */
		set_mode(get_mode_control_air());
		/* Ghi lai thoi gian tu luc bat dau on dieu hoa */
		set_request_check_air();				
	}
	else
	{
		memcpy((uint8_t *)&air_cmd.cmd_content[0],(uint8_t *)&table_control[(air_id - 1) * 2].cmd_content[0],table_control[(air_id - 1) * 2].len_cmd);
		air_cmd.len_cmd = table_control[(air_id - 1) * 2].len_cmd;
		/* Xoa canh bao check dieu hoa van hanh */
		clear_request_check_air();		
	}
	/* thuc hien viec dieu khien */
	if(air_id == SHARP_AIR)
	{
			control_air_sharp();
	}
	/* sau khi dieu khien gan lai trang thai cua cac dieu hoa */
	status_air = cmd;	
}

void re_ctrl_air(void)
{
	uint8_t cmd = (do_getvalue())?CMD_ON_AIR:CMD_OFF_AIR;
	if(re_ctrl)
		control_air(cmd,air_id);
}
