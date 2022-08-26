#ifndef RS485_H
#define RS485_H

#include "stdint.h"

#define TIMEOUT_CHECK 	1
#define TIMEOUT_ABOART	0

#define ID485_DETERMINE_PORT          GPIOB
#define ID485_DETERMINE_PIN           GPIO_PIN_13
#define ID485_DETERMINE_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

/* Definition for USARTx clock resources */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF1_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF1_USART2

                       /* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

#define RS485_CTRL_PORT                 GPIOB
#define RS485_CTRL_PIN                  GPIO_PIN_11
#define RS485_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_RECV_ENABLE()             HAL_GPIO_WritePin(RS485_CTRL_PORT, RS485_CTRL_PIN, GPIO_PIN_RESET)
#define RS485_SEND_ENABLE()             HAL_GPIO_WritePin(RS485_CTRL_PORT, RS485_CTRL_PIN, GPIO_PIN_SET)

                           /* for sms receiver */
#define RS485_MAX_RX_SIZE          		160 /* max leng is 134 character */
#define LENG_SMS_PACKAGE_FIRMWARE		138
#define LENG_FIRMWARE					64
#define RS485_SOH_CHAR             		':' /* Start char */
#define RS485_EOT_CHAR             		0x0A /* CR character is end of char */
                           /* error define */
#define SMS_GET_NONE_ERROR              0x00 /* None error */
#define SMS_GET_SIZE_ERROR              0x10 /* Size of sms error   */
#define SMS_GET_OPCODE_ERROR            0x20 /* Opcode of sms error */
#define SMS_GET_CAL_CRC_ERROR           0x30 /* Calculate crc error */
#define SMS_GET_CORRECT_TARGET_ERROR    0x40 /* Target not correct */
#define SMS_GET_DATA_ERROR              0x50 /* Data invalid */
#define SMS_GET_FILE_EXCEPT             0x60 /* Request data too larger*/

#define FIRMWARE_NOTY_UPDATE_FALSE      0x70 /* Update noty false*/
#define FIRMWARE_NOTY_UPDATE_OK      	0x80 /* Update noty false*/

#define FIRMWARE_PACKAGE_LOST			0x90 /*Lost data from package */
#define FIRMWARE_UPDATE_PACKAGE_FALSE	0xA0 /*Update data false */
#define FIRMWARE_UPDATE_PACKAGE_OK	    0xB0 /*Update data false */
																	
#define SMS_ERROR                       0x01 /*Error */
#define SMS_NONE_ERROR                  0x00 /*Error */

						  /* error define for control */
#define SMS_CONTROL_OK                  0x00 /* None error */
#define SMS_CONTROL_WARNIG_AIR1         0x01 /* Warnig Air1 */
#define SMS_CONTROL_WARNIG_AIR2         0x02 /* Warnig Air2*/
#define SMS_CONTROL_ONOFF_DISABLE       0x03 /* Air disable on or off because it not pass 15 minute*/						  

                         /* Opcode define */
#define SMS_OPCODE_START_UP      		0x00 /* Opcode query status */
#define SMS_OPCODE_QUERY_STATUS         0x01 /* Opcode query status */
#define SMS_OPCODE_CONTROL              0x02 /* Opcode control  */
#define SMS_OPCODE_UPDATE_FIRMWARE_NOTY	0x03 /* Opcode firmware noty */
#define SMS_OPCODE_FIRMWARE_PACKET      0x04 /* Opcode firmware packet */					 
#define SMS_OPCODE_FIRMWARE_CHECK       0x05 /* Opcode firmware check */
#define SMS_OPCODE_RESET_MODULE		    0x06 /* Opcode reset module */
						 /* Address of slave */
#define ADDRESS_FANCON_BOARD            0x01 /* Address of fan control board */
#define ADDRESS_AIRCON1_BOARD           0x02 /* Address of condition control board */
#define ADDRESS_AIRCON2_BOARD           0x03 /* Address of condition control board */

/*************************************** For debug *******************************************/
#define USART_DEBUG                      USART1
#define USART_DEBUG_CLK_ENABLE()         __HAL_RCC_USART1_CLK_ENABLE()
#define USART_DEBUG_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART_DEBUG_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART_DEBUG_FORCE_RESET()        __HAL_RCC_USART1_FORCE_RESET()
#define USART_DEBUG_RELEASE_RESET()      __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USART_DEBUG_TX_PIN               GPIO_PIN_9
#define USART_DEBUG_TX_GPIO_PORT         GPIOA
#define USART_DEBUG_TX_AF                GPIO_AF1_USART1
#define USART_DEBUG_RX_PIN               GPIO_PIN_10
#define USART_DEBUG_RX_GPIO_PORT         GPIOA
#define USART_DEBUG_RX_AF                GPIO_AF1_USART1

extern uint8_t	 debug_arr[100];
extern uint8_t	 debug_len;

void    ConvertUINT8ToHEXCHAR(uint8_t *temp, uint8_t num);
uint8_t ASCToHEXCHAR(uint8_t num);

void    RS485_Init(void); /* Initialize function */
void    RS485_Listen(void); /* Initialize to listen from rs485 */
void    RS485_GetRequest(void); /* get sms */
void    RS485_GetChar(void); /* get one characcter */
int32_t RS485_InMessageHandler(char *data, uint8_t len); /* Handle request from message received rs485 */
void    RS485_Respone(uint8_t *data,uint8_t opcode, uint8_t len); /* return error from sms receiver */
                          /* Mot so ham bo tro */
void 	rs485_reinit(void);
		/* For debug */
uint8_t uart_debug_init(void);
void 	debug_send(uint8_t *Data, uint8_t len);
void 	debug_putc(uint8_t Data);
/* mot so ham bo tro */
uint8_t get_mode_control_air(void);
uint8_t get_temp_set_air(void);
uint8_t get_fanlevel_air(void);
uint8_t get_swing_air(void);

void get_id485_module(void);

#define FIRMWARE_PACKAGE_OK										0
#define WRONG_FIRMWARE_PACKAGE_FORMAT					1
#define LOST_FIRMWARE_PACKAGE									2
#define SAVE_FIRMWARE_PACKAGE_FAIL						4
#define SAVE_STATUS_UPDATE_FAIL								8
#endif /* RS485_H */
