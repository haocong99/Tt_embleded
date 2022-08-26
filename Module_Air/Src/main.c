/**
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComIT/Src/main.c 
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    27-May-2016
  * @brief   This sample code shows how to use UART HAL API to transmit
  *          and receive a data buffer with a communication process based on
  *          IT transfer. 
  *          The communication is done using 2 Boards.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "main.h"
#include "ringbuf.h"
#include <string.h>
#include "rs485.h"
#include "do.h"
#include "stm32f0xx_hal.h"
#include "SPI.h"
#include "S25FL1.h"
#include "bootloader.h"
#include "Time.h"
#include "stm32f0xx_hal_iwdg.h"
#include "data_base.h"
#include "temperature.h"
#include "ctrl_air.h"
#include "system_start.h"
/** @addtogroup STM32F0xx_HAL_Examples
  * @{
  */

/** @addtogroup UART_TwoBoards_ComIT
  * @{
  */ 
/* variable for control air condition */
static uint32_t the_last_time = 0;
static uint8_t  count_time  = 0 ;
extern uint8_t rs485_have_data_flag;
extern uint8_t debug_have_data_flag;
extern uint8_t start_flag;
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* STM32F0xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Low Level Initialization
    */
	uint8_t count_start_false = 0;
	int8_t check_init = -1;
	uint32_t i = 0;
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 
  
    HAL_Init();
    /* Configure the system clock to 48 MHz */
    SystemClock_Config();
	/* Initialize uart debug */
	uart_debug_init();
	/* debug listen */
	debug_listen();
	/* khoi tao */
	watchdog_ex_initialize();
	/* setup timer */
	timer16_init();
	air_init();	
	ID_module_air();
	do
	{
		check_init =  config_init();
		count_start_false ++;
	}while(count_start_false < NUMBER_TRY_INIT && check_init < 0);
	sensor_temp_init();
	rs485_init();  
	rs485_listen();		
	/* test he thong */
	test_system();
	the_last_time = HAL_GetTick();
	while (1)
    {		
		/* xu ly luong 485 */
		if(rs485_have_data_flag){
			RS485_InMessageHandler(&RS485RxBuf[0],getBufferLen,SMS_FROM_485);
			rs485_have_data_flag = 0;
			getBufferLen = 0;
		}
		if(debug_have_data_flag){
			RS485_InMessageHandler(&DebugRxBuf[0],DebugBufferLen,SMS_FROM_DEBUG);
			debug_have_data_flag = 0;
			DebugBufferLen = 0;
		}
		if(HAL_GetTick() - the_last_time >= 2000){ /* 200 ms doc 1 lan */
			the_last_time = HAL_GetTick();
			test_ir();	
			tempearture_read();
			if(start_flag)
			{
				package_and_send_message();
			}
		}
		
    }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI/2)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 8000000
  *            PREDIV                         = 1
  *            PLLMUL                         = 12
  *            Flash Latency(WS)              = 1
  * @param  None
  * @retval None
  */
uint8_t type_clock_source  = 0;
static void SystemClock_Config(void)
{
	uint8_t result_check = HAL_OK;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	
	type_clock_source = RCC_CFGR_SWS_HSE;
	__PWR_CLK_ENABLE();

	/* No HSE Oscillator on Nucleo, Activate PLL with HSI/2 as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;	
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;	
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	result_check = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if (result_check != HAL_OK)
	{
		RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
		RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
		type_clock_source = RCC_CFGR_SWS_HSI;
		HAL_RCC_OscConfig(&RCC_OscInitStruct);
	}else{

	}
	/* Select PLL as system clock source and configure the HCLK, PCLK1 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1)!= HAL_OK)
	{
		/*Error_Handler();*/
	}
}
uint8_t get_type_clock(void)
{
	return type_clock_source;
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
