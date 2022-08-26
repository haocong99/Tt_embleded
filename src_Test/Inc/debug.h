#ifndef __DEBUG_H
#define __DEBUG_H

#include "std_header.h"
#include "ringbuf.h"

#define PC_USART_BAUDRATE 				    115200

#define PC_USART											USART1
#define PC_USART_GPIO									GPIOA
#define PC_USART_TX						   		 	GPIO_PIN_9 		
#define PC_USART_RX						    		GPIO_PIN_10

#define USARTx_TX_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_CLK_ENABLE()					__HAL_RCC_USART1_CLK_ENABLE()

#define USARTx_Debug_IRQn                   USART1_IRQn
#define USARTx_Debug_IRQHandler             USART1_IRQHandler	 



extern UART_HandleTypeDef USART_Debug;
extern float led_duty;
void debug_init(void);
void UART_PutByte(uint8_t byte);
void UART_PutByteArray(char *arr, uint16_t len);
void debug_deinit(void);
uint8_t debug_getchar(void);
void usart_debug_listen(void);

#define RS485_BAUDRATE 				            115200

#define RS485_USART												USART2
#define RS485_USART_GPIO									GPIOA
#define RS485_USART_TX						   		 	GPIO_PIN_2 		
#define RS485_USART_RX						    		GPIO_PIN_3
#define RS485_CTRL						   		 			GPIO_PIN_1 		

#define RS485_TX_GPIO_CLK_ENABLE() 		    __HAL_RCC_GPIOA_CLK_ENABLE()
#define RS485_RX_GPIO_CLK_ENABLE() 		    __HAL_RCC_GPIOA_CLK_ENABLE()
  
#define RS485_CLK_ENABLE()					      __HAL_RCC_USART2_CLK_ENABLE()

#define RS485_WRITE_ENABLE() 				       HAL_GPIO_WritePin(RS485_USART_GPIO,RS485_CTRL, GPIO_PIN_SET)
#define RS485_READ_ENABLE() 				       HAL_GPIO_WritePin(RS485_USART_GPIO,RS485_CTRL, GPIO_PIN_RESET)

#define RS485_IRQn                   				USART2_IRQn
#define RS485_IRQHandler             				USART2_IRQHandler	 
typedef struct{
	uint8_t smoke_alram: 1;
	uint8_t fire_alarm: 1;
	uint8_t drown_alarm: 1;
	uint8_t indoor_temp_alarm: 2;
	uint8_t indoor_huni_alarm: 2;
	uint8_t outdoor_temp_alarm: 2;
	uint8_t outdoor_humi_alarm: 2;
	uint8_t reserve_bits: 5;
}eviroment_alram_t;

typedef struct{
	uint8_t flash_error: 1;
	uint8_t sensor_error: 1;
	uint8_t aircond_error: 1;
	uint8_t lock_module_error: 1;
	uint8_t module_disconnect: 1;
	uint8_t reserve_bits: 3;
}aircond_error_t;

typedef struct{
	uint8_t flash_error: 1;
	uint8_t fan1_error: 3;
	uint8_t fan2_error: 3;
	uint8_t fan3_error: 3;
	uint8_t fan4_error: 3;
	uint8_t lock_moudlle_eroor: 1;
	uint8_t module_disconnect: 1;
	uint32_t reserve_bits: 17;
}fan_error_t;

typedef struct{
	uint8_t indoor_smoke: 1;
	uint8_t indoor_fire: 1;
	uint8_t drown_ss_alarm: 1;
	uint8_t indoor_temp_alarm: 1;
	uint8_t fan4_error: 1;
	uint8_t normal: 1;
	uint8_t high_alarm: 1;
	uint8_t disconnect: 1;
	uint8_t indoor_humi_alarm: 1;
}enviroment_alarm_t;

typedef struct{
	uint32_t unix_time;
	int16_t indoor_temp;
	int16_t indoor_humi;
	uint8_t indoor_th_ss_status;
	int16_t outdoor_temp;
	int16_t outdoor_humi;
	int8_t outdoor_th_ss_status;
	int8_t smoke_ss_status;
	uint8_t drown_ss_status;
	int8_t siren_status;
	uint16_t dc_ch1_volt;
	uint16_t dc_ch2_volt;
	uint16_t dc_ch3_volt;
	uint8_t door1_status;
	uint8_t door2_status;
	uint8_t motion_ss_status;
	uint8_t glass_ss_status;
	uint8_t fan1_status;
	uint8_t fan2_status;
	uint8_t fan3_status;
	uint8_t fan4_status;
	int16_t fan1_current;
	int16_t fan2_current;
	int16_t fan3_current;
	int16_t fan4_current;
	uint8_t fan1_error;
	uint8_t fan2_error;
	uint8_t fan3_error;
	uint8_t fan4_error;
	uint8_t lock_fan_status;
	uint8_t connect_fan_status;
	uint8_t aircond1_status;
	uint8_t aircond2_status;
	uint8_t control_temp_aircond1;
	uint8_t control_temp_aircond2;
	aircond_error_t aircond1_error;
	aircond_error_t aircond2_error;
	uint8_t lock_aircond1_status;
	uint8_t lock_aircond2_status;
	uint8_t connect_aircond1_status;
	uint8_t connect_aircond2_status;
	uint8_t cool_mode;
	int8_t expect_temp;
	int8_t power_module;
	int8_t detect_ac_grid;
	int8_t dectect_gen;
	int8_t dectect_load;
	int8_t load_status;
	uint16_t phaseL1_volt;
	uint16_t phaseL1_current;
	uint16_t phaseL1_freq;
	uint8_t phaseL1_capacity_factor;
	uint8_t phaseL1_eastron_status;
	uint16_t phaseL2_volt;
	uint16_t phaseL2_current;
	uint16_t phaseL2_freq;
	uint8_t phaseL2_capacity_factor;
	uint8_t phaseL2_eastron_status;
	uint16_t phaseL3_volt;
	uint16_t phaseL3_current;
	uint16_t phaseL3_freq;
	uint8_t phaseL3_capacity_factor;
	uint8_t phaseL3_eastron_status;
	uint16_t lock_module_status;
	uint8_t connect_power_module_status;
	uint16_t fuel_level;
	uint16_t oil_level;
	uint16_t water_temp;
	uint16_t ats_mode;
	uint16_t bat_12v_gen;
	uint16_t ats_48v;
	uint8_t charging_config;
	uint8_t charging_status;
	uint8_t charging_mode;
	uint8_t charging_error;
	uint8_t aircond1_alram;
	uint8_t aircond2_alram;
	uint32_t fan_warning;
	eviroment_alram_t enviroment_alram;
	uint16_t security_alram;
	uint8_t main_module_alram;
	uint16_t grid_alram;
	uint16_t gen_volt_alram;
	uint16_t load_volt_alram;
	uint16_t load_current_alram;
	uint16_t load_freq_alram;
	uint16_t unnormal_volt_alram;
	uint16_t gen_warning;
	uint16_t daocat_alram;
	uint8_t discharge_alram;
	uint8_t ats_alram;
	uint8_t dc_volt_alram;
	uint8_t last_aircond1_alram;
	uint8_t last_aircond2_alram;
	uint8_t last_fan_alram;
	eviroment_alram_t last_enviroment_alram;
	uint16_t last_security_alram;
	uint8_t last_main_module_alram;
	uint16_t last_grid_alram;
	uint16_t last_gen_volt_alram;
	uint16_t last_load_volt_alram;
	uint16_t last_load_current_alram;
	uint16_t last_load_freq_alram;
	uint16_t last_unnormal_volt_alram;
	uint16_t last_gen_alram;
	uint16_t last_daocat_alram;
	uint8_t last_discharge_alram;
	uint8_t last_ats_alram;
	uint8_t last_dc_volt_alram;
	// bo sung
	// Trang thai ket noi server
	uint8_t connect_server;
	// Password
	char password[9];
	// Nhiet do tram
	uint16_t temp_station;
	// Login
	uint8_t is_loggin;
	} status_pack_t;
uint8_t is_connect_server(void);
uint8_t is_have_alarm(void);
	
#define NO_MSG	0
#define HAVE_MSG	1
#define VALUE_TIMEOUT_MSG 200
#define MAX_LEN_PACK 30
#define MIN_LEN_PACK 5 
typedef struct{
	uint8_t data[MAX_LEN_PACK];
	uint16_t len;
	uint16_t timeout;
	uint8_t status_sms;
}data_receiver;
extern data_receiver uart_data;
extern status_pack_t status_data; 
extern UART_HandleTypeDef USART_Rs485;
void rs485_init(void);
void rs485_PutByte(uint8_t byte);
void rs485_PutByteArray(char *arr);
void usart_rs485_listen();
uint8_t rs485_getchar(void);
void proccess_msg(void);
#define DBG_PRINTF      printf
#define DBG_NPRINTF     rs485_PutByteArray
#endif
