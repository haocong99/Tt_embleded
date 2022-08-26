/**
 ******************************************************************************
 * @file    /ctOSv1/include/leds.h
 * @author  Clever Things JSC.
 * @version 1.0
 * @date    Apr 10, 2016
 * @brief   This file contained the headers of  handlers
 * @history
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE IS PROPERTY OF CLEVERTHINGS JSC., ALL CUSTOMIZATION
 * IS NOT ALLOWED. AS A RESULT, CLEVERTHINGS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE MODIFIED CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS
 * OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2016 CleverThings JSC.</center></h2>
 ******************************************************************************
 */

#include "leds.h"
#include "debug.h"
#include "utils.h"
#include "stdint.h"
#include "stdbool.h"

void led_init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	LED_SYS_G_CLK_ENABLE();
	LED_SYS_R_CLK_ENABLE();
	LED_ERR_G_CLK_ENABLE();
	LED_ERR_R_CLK_ENABLE();
	LED_NET_G_CLK_ENABLE();
	LED_NET_R_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = LED_SYS_G_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_SYS_G_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED_SYS_R_Pin;
  HAL_GPIO_Init(LED_SYS_R_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED_ERR_G_Pin;
  HAL_GPIO_Init(LED_ERR_G_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED_ERR_R_Pin;
  HAL_GPIO_Init(LED_ERR_R_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED_NET_G_Pin;
  HAL_GPIO_Init(LED_NET_G_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED_NET_R_Pin;
  HAL_GPIO_Init(LED_NET_R_Port, &GPIO_InitStruct);
}
void led_show(void)
{

	static uint64_t last_time = 0;
	if(get_sys_tick() - last_time >= 500){
		// Cho led system
		HAL_GPIO_WritePin(LED_SYS_R_Port, LED_SYS_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_SYS_G_Port, LED_SYS_G_Pin,GPIO_PIN_SET);	
		// Bat den xanh 
		HAL_GPIO_WritePin(LED_NET_R_Port, LED_NET_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_NET_G_Port, LED_NET_G_Pin,GPIO_PIN_SET);		
		// Bat den xanh 
		HAL_GPIO_WritePin(LED_ERR_R_Port, LED_ERR_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_ERR_G_Port, LED_ERR_G_Pin,GPIO_PIN_SET);			
		last_time = get_sys_tick();
	}
#if 0	
	if(is_connect_server()){
		// Bat den xanh 
		HAL_GPIO_WritePin(LED_NET_R_Port, LED_NET_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_NET_G_Port, LED_NET_G_Pin, GPIO_PIN_RESET);		
	}else{
		// Bat den do 
		HAL_GPIO_WritePin(LED_NET_R_Port, LED_NET_R_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_NET_G_Port, LED_NET_G_Pin, GPIO_PIN_SET);	
	}
	if(is_have_alarm()){
		HAL_GPIO_WritePin(LED_ERR_R_Port, LED_ERR_R_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_ERR_G_Port, LED_ERR_G_Pin, GPIO_PIN_SET);	
	}else{
		HAL_GPIO_WritePin(LED_ERR_R_Port, LED_ERR_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_ERR_G_Port, LED_ERR_G_Pin, GPIO_PIN_RESET);		
	}
#endif	
}