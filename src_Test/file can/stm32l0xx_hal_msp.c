/**
  ******************************************************************************
  * @file    Templates/Src/stm32l0xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]


  @endverbatim
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
#include "stm32l0xx_hal.h"
#include "gps.h"
/** @addtogroup STM32L0xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */
extern UART_HandleTypeDef USART_Debug;
extern UART_HandleTypeDef USART_GSM;
extern UART_HandleTypeDef USART_GPS;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is eventually modified by the user  */

}
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
	GPIO_InitTypeDef  GPIO_InitStruct;	
	if(huart->Instance == GPS_USART){
		  /*##-1- Enable peripherals and GPIO Clocks #################################*/
		  /* Enable GPIO TX/RX clock */
		  GPS_TX_GPIO_CLK_ENABLE();
		  GPS_RX_GPIO_CLK_ENABLE();
		  
		  /*##-2- Configure peripheral GPIO ##########################################*/  
		  /* UART TX GPIO pin configuration  */
		  GPIO_InitStruct.Pin       = GPS_USART_TX;
		  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		  GPIO_InitStruct.Pull      = GPIO_PULLUP;
		  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
		  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;

		  HAL_GPIO_Init(GPS_USART_GPIO, &GPIO_InitStruct);

		  /* UART RX GPIO pin configuration  */
		  GPIO_InitStruct.Pin = GPS_USART_RX;
		  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
		  HAL_GPIO_Init(GPS_USART_GPIO, &GPIO_InitStruct);
		 /* Interrupt setup */
		  /*##-3- Configure the NVIC for UART ########################################*/
		  /* NVIC for USART */
		  HAL_NVIC_SetPriority(USARTx_GPS_IRQn, 3, 0);
		  HAL_NVIC_EnableIRQ(USARTx_GPS_IRQn);		
	}
}
/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();
}
/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is eventually modified by the user */

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
