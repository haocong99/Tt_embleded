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

#ifndef LEDS_H
#define LEDS_H

#define LED_SYS_G_Pin 							GPIO_PIN_15
#define LED_SYS_G_Port 							GPIOA
#define LED_SYS_G_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()

#define LED_SYS_R_Pin 							GPIO_PIN_10
#define LED_SYS_R_Port 							GPIOC
#define LED_SYS_R_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()

#define LED_ERR_R_Pin 							GPIO_PIN_11
#define LED_ERR_R_Port 							GPIOC
#define LED_ERR_R_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()

#define LED_ERR_G_Pin 							GPIO_PIN_12
#define LED_ERR_G_Port 							GPIOC
#define LED_ERR_G_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()


#define LED_NET_G_Pin  GPIO_PIN_6
#define LED_NET_G_Port GPIOB
#define LED_NET_G_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()

#define LED_NET_R_Pin  GPIO_PIN_2
#define LED_NET_R_Port GPIOD
#define LED_NET_R_CLK_ENABLE()			__HAL_RCC_GPIOD_CLK_ENABLE()

void led_init(void);
void led_show(void);
#endif
