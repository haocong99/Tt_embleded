/**
 ******************************************************************************
 * @file    /ctOSv1/include/monitor.h
 * @author  Clever Things JSC.
 * @version 1.0
 * @date    Apr 9, 2016
 * @brief   This file contained the headers of system monitor handlers
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

#ifndef __MONITOR_H
#define __MONITOR_H
#include "stdint.h"
typedef enum {
    BATT_EMPTY = 0,
    BATT_LEVEL_1,
    BATT_LEVEL_2,
    BATT_LEVEL_3,
    BATT_LEVEL_4,
    BATT_FULL,
} m1_battery_level_state_t;
typedef enum {
    BATT_LOW = 0,
    BATT_NORMAL
} m2_battery_level_state_t;
typedef enum {
  E_SYSTEM,
  E_CTL_LED_CHANGE_STATE,
  E_DAY_NIGHT_CHANGE,   // VSolar < 4V
  E_NIGHT_DAY_CHANGE,    // VSolar >= 4V
  E_OPERATION_MODE_CHANGE, 
  E_LED_ERROR,
  E_BAT_ERROR,
  E_BAT_NORMAL,
  E_LED_NORMAL,
  E_UPDATE_STATUS
} OS_EVENT;
typedef enum {
  SAVING_MODE = 0,
  DEEP_SAVING_MODE,
  NORMAL_MODE
} device_mode_t;
typedef enum{
  INIT_MODE = 0,
  NIGHT_MODE,
  DAY_MODE
} RUNNING_MODE;

extern volatile RUNNING_MODE current_mode;
void SystemMonitor(void);
uint8_t GetOSEvent(OS_EVENT e);

void SetOSEvents(OS_EVENT e);

void ClearOSEvent(OS_EVENT e);
RUNNING_MODE get_current_mode(void);
uint8_t GetLEDStatus(void);
uint16_t GetSolarVoltage(void);
uint16_t GetBatteryVoltage(void);
uint16_t GetLEDCurrent(void);
m1_battery_level_state_t mode1_get_battery_state(void);
m2_battery_level_state_t mode2_get_battery_state(void);
device_mode_t get_device_op_mode (void);
void led_measure_current (void);
void LED_Check(void);
 void set_device_op_mode(device_mode_t mode); 

#endif // __MONITOR_H
