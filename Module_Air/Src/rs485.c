#include "main.h"
#include "rs485.h"
#include "ringbuf.h"
#include <string.h>
#include <stdlib.h>
#include "do.h"
#include "rs485.h"
#include "crc.h"
#include "bootloader.h"
#include "S25FL1.h"
#include "temperature.h"
#include "Time.h"
#include "ctrl_air.h"
#include "system_start.h"
uint8_t	 	debug_arr[100];
uint8_t	 	debug_len;
UART_HandleTypeDef UartHandle;
UART_HandleTypeDef UartDebug;

uint8_t RS485RxBuf[RS485_MAX_RX_SIZE];
uint8_t getBufferLen = 0;
uint8_t byte_receiver = 0;

uint8_t DebugRxBuf[RS485_MAX_RX_SIZE];
uint8_t DebugBufferLen = 0;
uint8_t Dbyte_receiver = 0;
uint8_t start_flag = 0;
char module_uid[32];
char module_hardware[32];
char ID_module[20];
uint8_t timeout_rs485 = 0;
uint8_t timeout_debug = 0;
static uint8_t request_response = 0;
extern char uid_mcu_arr[50];
extern char uid_flash_arr[50];
/**
  * @brief  Setup RS485 ready, recever byte one byte from interrupt uart receiver
  *         
  * @param  none
  * @retval none
  */
void rs485_listen(void)
{
	RS485_RECV_ENABLE();
	HAL_UART_Receive_IT(&UartHandle, &byte_receiver, 1);
}
/**
  * @brief  Send one string via rs485
  *         
  * @param  uint8_t *data : poiter, point to data to send
  * 		uint32_t len : Number of byte to send
  * @retval none
  */
void RS485_Write(uint8_t *data, uint32_t len)
{
	RS485_SEND_ENABLE();
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&":", 1, 0x1F);
	HAL_UART_Transmit(&UartHandle, data, len, 0xFFFF);
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&"\r\n", 2, 0x1F);
	rs485_listen();
}
/**
  * @brief  Initialize UART1
  *         
  * @param  None
  * 		
  * @retval none
  */
void rs485_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
    RS485_GPIO_CLK_ENABLE();
	
	/* initialize air 1*/
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = RS485_CTRL_PIN  ;
	HAL_GPIO_Init(RS485_CTRL_PORT , &GPIO_InitStructure);
	RS485_RECV_ENABLE();
	
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
  - Word Length = 8 Bits
  - Stop Bit = One Stop bit
  - Parity = None
  - BaudRate = 9600 baud
  - Hardware flow control disabled (RTS and CTS signals) */
	USARTx_CLK_ENABLE();
	UartHandle.Instance        = USARTx;

	UartHandle.Init.BaudRate   = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
	
	}
}

/**
  * @brief Convert one character ASCII to one hexa number.
  * @param  uint8_t num : Hexa character.
  * @retval temp : hexa number after convert.
  */
uint8_t ASCToHEXCHAR( uint8_t num)
{
	uint8_t temp = 0;
	if (num >= 0x30 && num <= 0x39) /* Character from '0' to '9' */
	{
		temp = num - 0x30; 
	}
	else /* Character from 'A' to 'F' */
	{
		temp = num - 0x37;

	}
	
	return temp;
}
/**
  * @brief Convert one uint8_t to 2 bytes ASCII
  * @param  uint8_t num : the number need to convert
  * uint8_t *temp : One pointer point to where storage twos bytes after convert
  * @retval none
  */

void ConvertUINT8ToHEXCHAR(uint8_t *temp, uint8_t num)
{
	uint8_t high = num >> 4;
	uint8_t low = num & 0x0F;
	
	if (high <= 9)  /* Hexa number from 0 to 9 */
	{
		temp[0] = high + 0x30;
	}
	else  /* Hexa number from A to F */
	{
		temp[0] = high + 0x37;
	}
	if (low <= 9)
	{
		temp[1] = low + 0x30;
	}
	else
	{
		temp[1] = low + 0x37;
	}    
}
/**
  * @brief Respone from slave after excuted request from master
  * 	
	---------------------------------------------
	| SOT | Address | Opcode | Data | CRC | EOT |
	---------------------------------------------     
  * @param uint8_t *Data: Data to send to master
		   uint8_t len  : Number of byte data will be sent to master.
  * @retval error code or none 
  */
void    RS485_Respone(uint8_t *data, uint8_t len)
{
	/* Ghi du lieu ra rs485 */
	RS485_Write(data,len);
	/* debug send */
	debug_send(data,len);
}
extern flash_t 	flash;
void package_and_send_message(void)
{
	uint8_t len = 0;
	char module_info[200];
	len=sprintf((char *)&module_info[0],"AT+INFOR=%s,%s,%d,%s,%s,%d,%d,%d\n",&module_uid[0],&module_hardware[0],(get_type_clock() == RCC_CFGR_SWS_HSE)?1:0,&uid_mcu_arr[0],&uid_flash_arr[0],flash.status,get_temperature(),get_id485_module());
	RS485_Respone((uint8_t *)&module_info[0], strlen((char *)&module_info[0]));
}
/**
  * @brief This fuction handle request message from RS485
  * +, Step 1 : Get Opcode, Address of device, CRC from message
  * +, Check error from message by calling function   RS485_GetError(sms), 
       the message only handle if it has no error;
  * +, Call corespone function base on Opcode received.
  * @param char *data : Pointer point to message received from rs485.
  * uint8_t len : length of message.
  * @retval 0
  */
int32_t RS485_InMessageHandler(uint8_t *data, uint8_t len, uint8_t source)
{
	int8_t check_t = 0;
	uint8_t request_answer_uid = 1;
	uint8_t buff[100]; // buffer chua du lieu dau vao
	uint8_t buffer_write[100]; // Buffer trung gian
	uint8_t buffer_read[100]; // Buffer trung gian
	char module_id[50];
	int16_t start_pos = -1;
	int16_t end_pos   = -1;
	uint8_t count = 0;
	uint8_t error_code;
	uint8_t len_resp = 0;
	/* so sanh do dai */
	if(len < 9)
	{
		memset((char *)&data[0],0,len);
		len = 0;
		return 0;
	}
	for(count = 0; count < len; count ++)
	{
		if(data[count] == ':')
		{
			start_pos = count + 1;
			break;
		}
	}
	if(count >= len - 2)
		return 0;
	for(count = start_pos; count < len - 1; count ++)
	{
		if(data[count] == '\r' && data[count + 1] == '\n')
		{
			end_pos = count;
			break;
		}
	}
	if(end_pos <= start_pos)
		return 0;	
	/* Lay du lieu */
	memset(&buff[0],0,100);
	memcpy(&buff[0],&data[start_pos],end_pos - start_pos);// gan du lieu nhan duoc tu data sang buff
	/* kiem tra code o day */
	if(strncmp((char *)&buff[0],"AT+START=1",10) == 0)
	{
		memset(&module_id[0],0,50);
		len = sprintf((char *)&module_id[0],"AT+MODULE=02");
		RS485_Respone((uint8_t *)&module_id[0], strlen((char *)&module_id[0]));
		HAL_Delay(100);		
		/*thong tin temp, ID_MODULE*/
		start_flag = 1;	
	}
	else if(strncmp((char *)&buff[0],"AT+MID=",7) == 0){
		/* Lay serial */
		start_pos = 7;
		memset(&buffer_write[0],0,100);
		count = start_pos;
		while( count < strlen((char *)&buff[7]) && buff[count] != ',')
		{
			buffer_write[count - start_pos] = buff[count];
			count ++;
		}
		/* hardware version */
		start_pos = ++count;
		// Kiem tra xem co con bytes nao k?
		if(strlen((char *)&buff[start_pos])>0){
			memcpy(&buffer_write[32],&buff[start_pos],strlen((char *)&buff[start_pos]));
		}else{
			// Bo qua
			return -1;
		}
	 check_t=write_uid_module(&buffer_write[0],&buffer_read[0]);	
		if(check_t == NO_ERROR){
			memcpy(&module_uid[0],&buffer_read[0],32);
			memcpy(&module_hardware[0],&module_hardware[32],32);
		}else{
			memset(&module_uid[0],'F',32);
			memset(&module_hardware[0],'F',32);
		}
	}else if(strncmp((char *)&buff[0],"AT+RESET=1",10) == 0){		
		timer16_stop();		
	}
	else{
	}
	memset((char *)&buff[0],0,len);		
	return 0;
}

void rs485_reinit(void)
{
	/* re-initialize rs485 */
	rs485_init(); 	
	rs485_listen();
}
/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USARTx)
	{
		RS485RxBuf[getBufferLen ++] = byte_receiver;
		rs485_listen();
		timeout_rs485 = 4;
    }else{
		DebugRxBuf[DebugBufferLen ++] = Dbyte_receiver;
		debug_listen();
		timeout_debug = 4;	
	}		
}

/**
  * @brief  Setup RS485 ready, recever byte one byte from interrupt uart receiver
  *         
  * @param  none
  * @retval none
  */
void debug_listen(void)
{
	HAL_UART_Receive_IT(&UartDebug, &Dbyte_receiver, 1);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart ->Instance == USART_DEBUG)
	{
			uart_debug_init();	
	}
	else if(huart ->Instance == USARTx)
	{
		/* initialize */		
		rs485_init();
		rs485_listen();
	}
	else
	{
		/* to do */
	}
}
	/* For debug only */
uint8_t uart_debug_init(void)
{

	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
  - Word Length = 8 Bits
  - Stop Bit = One Stop bit
  - Parity = None
  - BaudRate = 9600 baud
  - Hardware flow control disabled (RTS and CTS signals) */
	USART_DEBUG_CLK_ENABLE();
	UartDebug.Instance        = USART_DEBUG;
								
	UartDebug.Init.BaudRate   = 115200;
	UartDebug.Init.WordLength = UART_WORDLENGTH_8B;
	UartDebug.Init.StopBits   = UART_STOPBITS_1;
	UartDebug.Init.Parity     = UART_PARITY_NONE;
	UartDebug.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartDebug.Init.Mode       = UART_MODE_TX_RX;
	UartDebug.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 

	if(HAL_UART_Init(&UartDebug) != HAL_OK)
	{
		while(1);
	}
	
	return 0;
}
void 	debug_send(uint8_t *Data, uint8_t len)
{
	HAL_UART_Transmit(&UartDebug, (uint8_t *)&":", 1, 0x1F);
	HAL_UART_Transmit(&UartDebug, &Data[0], len, 0xFFFF);
	HAL_UART_Transmit(&UartDebug, (uint8_t *)&"\r\n", 2, 0x1F);
}