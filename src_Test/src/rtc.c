/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include <string.h>
#include <stdlib.h>
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_rcc.h"
#include "debug.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;
RTC_DateTypeDef  sdatestructure;
RTC_TimeTypeDef  stimestructure;
RTC_AlarmTypeDef salarmstructure;

/* RTC init function */
void MX_RTC_Init(void)
{
 /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */
 __HAL_RCC_RTC_ENABLE();
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {

  }

	uint32_t mark = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0);
	if(mark != 0xAA55){
		/** Initialize RTC and set the Time and Date 
		*/
		sTime.Hours = 10;
		sTime.Minutes = 15;
		sTime.Seconds = 0;
		sTime.TimeFormat = RTC_HOURFORMAT12_PM;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{

		}
		sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
		sDate.Month = RTC_MONTH_MAY;
		sDate.Date = 25;
		sDate.Year = 21;

		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{

		}	
		 /* Danh dau da ghi */
		HAL_PWR_EnableBkUpAccess();
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0xAA55);
		HAL_PWR_DisableBkUpAccess();		
	}else{
		/* do nothing */		
	}	

}
 
void rtc_set_time(struct tm *time)
{
    RTC_TimeTypeDef stime;
    RTC_DateTypeDef date;    
		if((time->tm_sec > 59)  || (time->tm_min > 59) || (time->tm_hour > 23) ||
			 (time->tm_mday > 31) || (time->tm_mon > 12) || (time->tm_year > 99))
		{
			return;
		}

		/**Initialize RTC and set the Time and Date 
			*/
		stime.Hours = time->tm_hour;
		stime.Minutes = time->tm_min;
		stime.Seconds = time->tm_sec;
		stime.TimeFormat = RTC_HOURFORMAT_24;
		stime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		stime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
		{
			//Error_Handler();
		}
		date.WeekDay = RTC_WEEKDAY_FRIDAY;
		date.Month = time->tm_mon;
		date.Date = time->tm_mday;
		date.Year = time->tm_year;
		
		if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
		{
			//Error_Handler();
		}
		return;
}

void rtc_get_datetime(struct tm *curr_time)
{
    RTC_TimeTypeDef stime;
    RTC_DateTypeDef date;
    hrtc.Instance = RTC;
		
    HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    curr_time->tm_hour = stime.Hours;
    curr_time->tm_min = stime.Minutes;
    curr_time->tm_sec = stime.Seconds;
    curr_time->tm_mday = date.Date;
    curr_time->tm_mon = date.Month;
    curr_time->tm_year = date.Year;
}
struct tm curr_time;
void read_rtc_time(void)
{
	rtc_get_datetime(&curr_time);
}
struct tm get_time(void)
{
	return curr_time;
}
uint32_t get_unix_time(void)
{
	struct tm _time;
	rtc_get_datetime(&_time);
	/* Lay thoi gian hien tai va convert ra unix time */
	_time.tm_year = _time.tm_year + 100;
	_time.tm_mon = _time.tm_mon - 1;	
	return mktime(&_time);
}

void convert_u32_to_server_time(uint32_t time_sec, struct tm *lastTime)
{
	struct tm *tim;
	if(time_sec) {
		tim = localtime((time_t *)&time_sec);
		memcpy(lastTime, tim, sizeof(struct tm));
	}
}
void getDateTime(char *buff){
	struct tm curr_time;
	rtc_get_datetime(&curr_time);
	sprintf(buff, "%04d%02d%02d%02d%02d%02d",2000+curr_time.tm_year, curr_time.tm_mon, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
	return;
}

uint32_t RTC_GetTimeInSec(void){
	RTC_TimeTypeDef sTime;
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	return (sTime.Hours*3600 + sTime.Minutes*60 + sTime.Seconds);
}


/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
