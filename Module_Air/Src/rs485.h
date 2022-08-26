#ifndef RS485_H
#define RS485_H

#include "stdint.h"

#define TIMEOUT_CHECK 	1
#define TIMEOUT_ABOART	0
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
#define RS485_MAX_RX_SIZE          		255 /* max leng is 134 character */
#define TYPE_FLASH_1					1
#define TYPE_FLASH_2					2

#define OK								0
#define ERROR							1
#define TIMEOUT							2

#define ON								1
#define OFF								0

#define FAN1							1
#define FAN2							2
#define FAN3							3
#define FAN4							4

#define SMS_FROM_DEBUG					1
#define SMS_FROM_485					2
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

#define USART_Debug                      USART1_IRQn
#define USART_IRQHandler                 USART1_IRQHandler

extern uint8_t	 debug_arr[100];
extern uint8_t	 debug_len;
extern uint8_t RS485RxBuf[RS485_MAX_RX_SIZE];
extern uint8_t getBufferLen;

extern uint8_t DebugRxBuf[RS485_MAX_RX_SIZE];
extern uint8_t DebugBufferLen;

void    ConvertUINT8ToHEXCHAR(uint8_t *temp, uint8_t num);
uint8_t ASCToHEXCHAR(uint8_t num);

void    rs485_init(void); /* Initialize function */
void    rs485_listen(void); /* Initialize to listen from rs485 */
void    RS485_GetRequest(void); /* get sms */
void    RS485_GetChar(void); /* get one characcter */
void 	RS485_Write(uint8_t *data, uint32_t len);

int32_t RS485_InMessageHandler(uint8_t *data, uint8_t len, uint8_t source); /* Handle request from message received rs485 */
void    RS485_Respone(uint8_t *data, uint8_t len); /* return error from sms receiver */
void 	get_crystal(void);
void 	set_uid(uint8_t *uid);
void set_hardwareversion(uint8_t *hardware);
void 	set_flash(uint8_t type_cmd, uint8_t result);
void 	response_current(void);
void 	set_request_response(uint8_t id);
void 	rs485_reinit(void);
		/* For debug */
uint8_t uart_debug_init(void);
void 	debug_listen(void);
void 	debug_send(uint8_t *Data, uint8_t len);

uint8_t get_type_clock(void);
void package_and_send_message(void); 
void ID_module_infor(void);
#endif /* RS485_H */
