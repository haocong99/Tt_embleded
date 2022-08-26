#ifndef CURRENT_MEAS_H
#define CURRENT_MEAS_H
#include "main.h"
/* Some define */
#define AIN_AIRCON1_PORT	GPIOB
#define AIN_AIRCON1_PIN	GPIO_PIN_1

#define AIN_AIRCON2_PORT	GPIOB
#define AIN_AIRCON2_PIN	GPIO_PIN_0

#define AIN_AIRCON1_CURRENT_CHANNEL ADC_CHANNEL_9
#define AIN_AIRCON2_CURRENT_CHANNEL ADC_CHANNEL_8

#define ADC_MAX							4096
#define VOLT_REFERENCE					3.3 /* Volt */
#define VOLTCONVERT_GAIN				VOLT_REFERENCE/ADC_MAX

#define MULTIL_GAIN						1000
#define NUM_CONVERT_WITHIN80MS			706

#define SENSORTIVE						66.0   /* mV/A */
//#define VOLTAGE_CALIB					2500 /* mV */
#define SCALE_VOLTAGE					5/3.3 /* Change input voltage from 5V to 3.3 Volt */

#define NUMBER_TIMES_CALIB				100.0

#define GAIN_CONVERT					SCALE_VOLTAGE * VOLTCONVERT_GAIN * MULTIL_GAIN
typedef struct {
	uint16_t  Current_Air1;
	uint16_t  Current_Air2;
} Current_t;

void ADC_Initialize(void);
//void Calib(void);
uint16_t AIRCON_1_GetCurrent(void);
uint16_t AIRCON_2_GetCurrent(void);
void ADC_DeInitialize(void);
void ADC_Restart(void);
void Caculate_Current(void);
void calib_current_air1(void);
void calib_current_air2(void);
void Calib_Current(void);
#endif
