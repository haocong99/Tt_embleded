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

#ifndef WATCHDOG_H
#define WATCHDOG_H

#define WDI_Pin GPIO_PIN_0
#define WDI_GPIO_Port GPIOA
#define WDI_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define WDI_TOGGLE()				HAL_GPIO_TogglePin(WDI_GPIO_Port, WDI_Pin)

void external_watchdog_init(void);
#endif
