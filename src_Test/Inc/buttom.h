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

#ifndef BUTTOM_H
#define BUTTOM_H

#define STML0_ROW1_Pin  		GPIO_PIN_10
#define STML0_ROW1_Port			GPIOB
#define STML0_ROW1_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define STML0_ROW1_STATUS() HAL_GPIO_ReadPin(STML0_ROW1_Port, STML0_ROW1_Pin)

#define STML0_ROW2_Pin			GPIO_PIN_2
#define STML0_ROW2_Port			GPIOB
#define STML0_ROW2_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define STML0_ROW2_STATUS() HAL_GPIO_ReadPin(STML0_ROW2_Port, STML0_ROW2_Pin)

#define STML0_ROW3_Pin  		GPIO_PIN_1
#define STML0_ROW3_Port			GPIOB 
#define STML0_ROW3_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define STML0_ROW3_STATUS() HAL_GPIO_ReadPin(STML0_ROW3_Port, STML0_ROW3_Pin)

#define STML0_ROW4_Pin			GPIO_PIN_5
#define STML0_ROW4_Port			GPIOC
#define STML0_ROW4_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()
#define STML0_ROW4_STATUS() HAL_GPIO_ReadPin(STML0_ROW4_Port, STML0_ROW4_Pin)

#define STML0_ROW5_Pin			GPIO_PIN_5
#define STML0_ROW5_Port			GPIOA
#define STML0_ROW5_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define STML0_ROW5_STATUS() HAL_GPIO_ReadPin(STML0_ROW5_Port, STML0_ROW5_Pin)

#define STML0_COL1_Pin  		GPIO_PIN_4
#define STML0_COL1_Port 		GPIOA
#define STML0_COL1_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define STML0_COL1_STATUS() HAL_GPIO_ReadPin(STML0_COL1_Port, STML0_COL1_Pin)

#define STML0_COL2_Pin  		GPIO_PIN_4
#define STML0_COL2_Port 		GPIOC
#define STML0_COL2_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()
#define STML0_COL2_STATUS() HAL_GPIO_ReadPin(STML0_COL2_Port, STML0_COL2_Pin)

#define STML0_COL3_Pin  		GPIO_PIN_7
#define STML0_COL3_Port			GPIOA
#define STML0_COL3_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define STML0_COL3_STATUS() HAL_GPIO_ReadPin(STML0_COL3_Port, STML0_COL3_Pin)

#define STML0_COL4_Pin			GPIO_PIN_6
#define STML0_COL4_Port			GPIOA
#define STML0_COL4_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define STML0_COL4_STATUS() HAL_GPIO_ReadPin(STML0_COL4_Port, STML0_COL4_Pin)

#define STML0_COL5_Pin			GPIO_PIN_0
#define STML0_COL5_Port			GPIOB
#define STML0_COL5_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define STML0_COL5_STATUS() HAL_GPIO_ReadPin(STML0_COL5_Port, STML0_COL5_Pin)

#define FILTER_BUTTOM 3
void button_scan(void);
char get_event_button(void);
void clear_event_button(void);
#endif
