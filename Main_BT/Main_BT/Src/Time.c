#include "main.h"
//#include "data_base.h"
#include "stm32f0xx_hal.h"
#include "Time.h"
#include "rs485.h"
#include "do.h"
#include "bootloader.h"
#include "stm32f0xx_hal_tim.h"
//#include "ctrl_air.h"
#include "sharp_air.h"
#include "DAIKIN_AIR.h"
#include "REETECH_AIR.h"
#include "FUNIKI_AIR.h"
#include "MITSUMISHI_AIR.h"
#include "GENERAL_AIR.h"
#include "NAGAKAWA_LC.h"
#include "NAGAKAWA_LM.h"
#include "MITSUMISHI_HEAVY_INDUSTRIES.h"
#include "PANASONIC.h"
#include "REETECH_YKR.h"
#include "CARRIER.h"
#include "GREE.h"
#include "GALANZ.h"
#include "NAGAKAWA_R51M.h"
#include "SAMSUNG.h"
#include "LG.h"
#include "TCL.h"

TIM_HandleTypeDef    htim3;
TIM_HandleTypeDef    htim16;

uint8_t  flag_timer		  = 0; /* Update timer flag */
uint8_t rs485_timeout			= 0;
uint8_t package_timeout         = 0;
extern   uint8_t update_firmware_request;
static uint32_t Count_OverFlow = 0;
uint32_t time_1s = 0;
uint8_t  request_read_sensor = 0;
uint32_t last_time_request_check_sensor = 0;
uint8_t turn_on_air_lost_connect = 0;
extern IWDG_HandleTypeDef 	hwwdg;
/* callback timer 3 handle */
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim3);
}
/* Private functions ---------------------------------------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
	if(htim->Instance == TIM3){
		/* check su kien */ 
		flag_timer = 1;
		/* tang thoi gian hien tai len 1*/	
	}
	else if(htim->Instance == TIM16)
	{
		/* turn on Time_flag */ 
		Count_OverFlow ++;
		WATCHDOG_REFRESH();
		HAL_IWDG_Refresh(&hwwdg);
	}else{
		/* to do */
	}
}
void timer3_init(void)
{
	__HAL_RCC_TIM3_CLK_ENABLE();
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = TIMER3_PRESCALER;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = TIMER3_PERIODE;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.RepetitionCounter = 0;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	/*enable nvic here*/
	HAL_NVIC_SetPriority(TIM3_IRQn, 2, 1);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
	}
	/* Interrupt start */
	HAL_TIM_Base_Start_IT(&htim3);
}
uint8_t  process_timeover(void)
{ 
	/* update firmware */
	if(update_firmware_request) /* dang trong qua trinh update firmware */
	{
		/* tang bien dem len */
		package_timeout ++;
		if(package_timeout > 50) /* qua 50s ma khong nhan duoc ban tin nao thi reset */
		{
			/* reset 485 */
			rs485_reinit();
			/* xoa bien dem */
			package_timeout = 0;
			/* xoa co bao */
			update_firmware_request = 0;
		}
		rs485_timeout = 0;
	}else{
		/* tang bien rs485 timeout and check */
		rs485_timeout ++;
		/* neu qua 3s ma no khong bi xoa => time out */
		if(rs485_timeout >= 200)
		{
			/* bat ca 2 dieu hoa len neu truoc do chua bat */
			if(!turn_on_air_lost_connect)
			{
				control_air(CMD_ON_AIR,air_id);
				turn_on_air_lost_connect = 1;
			}
			else
			{
				/* to do */
			};
			/* reset 485 */
			rs485_reinit();
			/* xoa co bao */
			rs485_timeout = 0;
		}else{
			/* to do */
		}
		if(time_1s - last_time_request_check_sensor > 3)
		{
			last_time_request_check_sensor = time_1s;
			request_read_sensor  = 1;
		}
	}
	return 0;
}
/* chi tinh gan chinh xac thoi */
void 	delay_us(uint32_t us) /* tao khoang thoi gian ~1us, yeu cau dau vao thoi gian khong nho hon 2us */
{
	/* con so nay chi mang tinh uoc luong */ 
	int64_t end = us * 6 - 8;// - 17;
	uint32_t start = 0;
	for(start = 0; start < end; start ++)
	{
		/* do nothing */
	};
}

/* callback timer 16 handle */
void TIM16_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim16);
}
	
void timer16_stop(void)
{
	/* Interrupt stop */
	HAL_TIM_Base_Stop_IT(&htim16);
	/* xoa bien dem thoi gian */
	Count_OverFlow = 0;
}
void timer16_resume(void)
{
	/* Interrupt start */
	HAL_TIM_Base_Start_IT(&htim16);
	/* xoa bien dem thoi gian */
	Count_OverFlow = 0;
}
void timer16_init(void)
{
	__HAL_RCC_TIM16_CLK_ENABLE();
	htim16.Instance = TIM16;
	htim16.Init.Prescaler = TIMER16_PRESCALER;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = TIMER16_PERIODE;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.RepetitionCounter = 0;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	/*enable nvic here*/
	HAL_NVIC_SetPriority(TIM16_IRQn, 1, 1);
	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM16_IRQn);
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
	{
	}
	/* Interrupt start */
	HAL_TIM_Base_Start_IT(&htim16);
}

uint32_t 	HAL_Timer_get(void)
{
	return __HAL_TIM_GET_COUNTER(&htim16);
}
void Delay(uint32_t x)
{
	uint32_t Time_Start = 0;
	uint32_t Time_End   = 0;
	uint32_t Time_Count = 0;
	uint32_t Time_eslap = 0;
	/* Start delay */
	Time_Count = Count_OverFlow;
	Time_Start = HAL_Timer_get();
	do{
		Time_End 	= HAL_Timer_get();
		if(Count_OverFlow > Time_Count)
			Time_eslap  = ((Count_OverFlow - Time_Count) *(TIMER16_PERIODE + 1) + Time_End) - Time_Start;
		else
			Time_eslap  = Time_End - Time_Start;
	}while(Time_eslap < x);
}
uint32_t get_time_s(void)
{
	return time_1s;
}

void 	timer_init(void)
{
	timer3_init();
}
