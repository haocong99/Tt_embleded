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
uint8_t re_ctrl = 1;
uint8_t update_firmware_request	= 0;
				/* mot so bien */
static uint8_t air_mode_set = 0; /* mode air */
uint8_t air_temp_set = 28; /* 23C */
static uint8_t fan_level_set = 0x01; /* fan level */
static uint8_t swing_set 	= 0x00; /* set off*/
						/* For RS485 */
UART_HandleTypeDef 							UartHandle;
						/* For Debug Only */
UART_HandleTypeDef 							UartDebug;

uint8_t	 debug_arr[100];
uint8_t	 debug_len;

uint8_t RS485RxBuf[RS485_MAX_RX_SIZE];
uint8_t getBuffer[RS485_MAX_RX_SIZE];
static uint8_t getBufferLen 				= 0;
static uint8_t gRS485RxChar 				= 0;
static struct ringbuf rxBuf;
extern uint8_t rs485_timeout;
uint8_t id485_module = ADDRESS_AIRCON1_BOARD;			
//uint8_t alarm_air = 1;
extern uint8_t Serial_number[50];
/**
  * @brief  Setup RS485 ready, recever byte one byte from interrupt uart receiver
  *         
  * @param  none
  * @retval none
  */
void RS485_Listen(void)
{
	RS485_RECV_ENABLE();
	HAL_UART_Receive_IT(&UartHandle, &gRS485RxChar, 1);
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
	HAL_Delay(1);
	HAL_UART_Transmit(&UartHandle, data, len, 0xFFFF);
	RS485_RECV_ENABLE();
}
/**
  * @brief  Initialize UART1
  *         
  * @param  None
  * 		
  * @retval none
  */
void RS485_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	RS485_GPIO_CLK_ENABLE();

	 ringbuf_init(&rxBuf, RS485RxBuf, RS485_MAX_RX_SIZE);
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART configured as follows:
  - Word Length = 8 Bits
  - Stop Bit = One Stop bit
  - Parity = None
  - BaudRate = 115200 baud
  - Hardware flow control disabled (RTS and CTS signals) */
	USARTx_CLK_ENABLE();
	UartHandle.Instance        = USARTx;

	UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
	if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	{
	
	}  
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
	
	}
	/* CTRL485 pin */
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = RS485_CTRL_PIN  ;
	HAL_GPIO_Init(RS485_CTRL_PORT , &GPIO_InitStructure); 
}
void RS485_GetChar(void)
{
	ringbuf_put(&rxBuf, gRS485RxChar);
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
  * @brief This fuction handle request message from RS485
  * +, Step 1 : Get Opcode, Address of device, CRC from message
  * +, Check error from message by calling function   RS485_GetError(sms), 
       the message only handle if it has no error;
  * +, Call corespone function base on Opcode received.
  * @param char *data : Pointer point to message received from rs485.
  * uint8_t len : length of message.
  * @retval 0
  */
int32_t RS485_InMessageHandler(char *data, uint8_t len)
{
	uint8_t 	opcode 						= 0x00; 
	uint8_t 	address 					= 0x00;
	uint8_t  	data_control 				= 0x00;
	uint8_t 	crc_receiver 				= 0x00;
	uint8_t 	crc_calculate				= 0x00;
	
	/* some variable for querry command */
	uint16_t temp = 0;
	uint8_t status   		 	 = 0;
	int8_t check_error           = 0;
	uint8_t error_code 			 = 0;
	char data_response[100]      ={0};
	char data_receiver[RS485_MAX_RX_SIZE];
	/* some variable for update firmware */
	static uint16_t  number_package = 0;
	static uint16_t	 before_package = 0; 	
	uint8_t  		 firmware[LENG_FIRMWARE];
	uint32_t 		 id_package = 0;
	uint8_t 		 len_data = len;
	uint8_t 		 len_data_send = 0;
	
	/* parse ban tin */
	rs485_timeout 	= 0;
	address        	= ASCToHEXCHAR(data[0])*16 + ASCToHEXCHAR(data[1]); /* Get Address from byte 0 and 1 of sms  */
	opcode         	= ASCToHEXCHAR(data[2])*16 + ASCToHEXCHAR(data[3]); /* Get Opcode from byte 2 and 3 of sms  */
	crc_receiver    = ASCToHEXCHAR(data[len_data - 2])*16 +  ASCToHEXCHAR(data[len_data - 1]); /* Get opcode from byte 4 and 5 of sms  */

	memset(data_response,0,30); /* alway memset array before use*/
	memcpy(&data_receiver[0],data,len);
	memset(data,0,RS485_MAX_RX_SIZE);
	
//	debug_send((uint8_t *)&data_receiver[0],len);
//	debug_send((uint8_t *)"\n",1);
	turn_on_air_lost_connect = 0;
	/* check address board */
	if(address != id485_module){
		
		//debug_len =sprintf((char *)&debug_arr[0],"Target board error \n");
		//debug_send((uint8_t *)&debug_arr[0],debug_len);
		return SMS_GET_CORRECT_TARGET_ERROR;
		
	}
	/* Calculate CRC */
	crc_calculate = Cal_CRC8((uint8_t *)&data_receiver[0],(uint32_t)(len - 2));
	if( crc_calculate != crc_receiver)
	{
		/* reset buffer */
		//debug_len =sprintf((char *)&debug_arr[0],"crc_calculate = %d, crc_receiver =%d \n",crc_calculate,crc_receiver);
		//debug_send((uint8_t *)&debug_arr[0],debug_len);
		memset(	data,0,RS485_MAX_RX_SIZE);
		return SMS_GET_CAL_CRC_ERROR;
	}
	
	switch(opcode) /* check what type of sms */
	{
		case SMS_OPCODE_QUERY_STATUS:
		{
			/* Get status */
			status   = do_getvalue();
			temp = get_temperature();
			error_code = get_error();
			/* Status air */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0], status);
			/* nhiet do */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[2],(temp >> 8));
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],(temp & 0xff));
			/* error_code */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[6],error_code);
			/* Nhiet do bat dieu hoa */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[8],get_temp_set_air());
			RS485_Respone((uint8_t *)&data_response[0],opcode, 10);
		}
		break;
		case SMS_OPCODE_CONTROL:
		{	
			data_control = ASCToHEXCHAR(data_receiver[4])*16 + ASCToHEXCHAR(data_receiver[5]); /* Get data from byte 4 and 5 of sms  */
			data_control &= 0x0F;
//			air_mode_set = ASCToHEXCHAR(data_receiver[6])*16 + ASCToHEXCHAR(data_receiver[7]);
			air_temp_set = ASCToHEXCHAR(data_receiver[8])*16 + ASCToHEXCHAR(data_receiver[9]);
			fan_level_set = ASCToHEXCHAR(data_receiver[10])*16 + ASCToHEXCHAR(data_receiver[11]);
			swing_set = ASCToHEXCHAR(data_receiver[12])*16 + ASCToHEXCHAR(data_receiver[13]);
			air_id = ASCToHEXCHAR(data_receiver[14])*16 + ASCToHEXCHAR(data_receiver[15]);
			//re_ctrl = (ASCToHEXCHAR(data_receiver[16])*16 + ASCToHEXCHAR(data_receiver[17]));
			error_code = get_error();
			/* for aircondition */
			if(data_control > CMD_ON_AIR )
			{
				return SMS_GET_DATA_ERROR;
			}
			if((air_temp_set > 30) || (air_temp_set < 16))
			{
				error_code = get_error();
				return SMS_GET_DATA_ERROR;
			}
		
			control_air(data_control,air_id);
			/* Get status */
			status  	 = do_getvalue();
			/* Status air */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0], status);
			/* error_code */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[2],error_code);
			RS485_Respone((uint8_t *)&data_response[0],opcode,4);	
		}
		break;
		case SMS_OPCODE_UPDATE_FIRMWARE_NOTY:
		{
			debug_len =sprintf((char *)&debug_arr[0],"Noty sms!\n");
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			/* check flash truoc */
			check_error = get_error();
			if(check_error & ERROR_FLASH)
			{
				debug_len =sprintf((char *)&debug_arr[0],"System error can't update!\n");
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				/* He thong co loi */
				error_code = SAVE_FIRMWARE_PACKAGE_FAIL;
				/* opcode = opcode + 0x80 */
				ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0],error_code);
				RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,2);
				return get_error();	
			}else{
				/* xoa sector dau tien */
				check_error = eraser_sector_external_flash(SECTOR_START_APP_STORE_EXT);
				if(check_error < 0){
					debug_len =sprintf((char *)&debug_arr[0],"Eraser sector false!\n");
					debug_send((uint8_t *)&debug_arr[0],debug_len);
					error_code = SAVE_FIRMWARE_PACKAGE_FAIL;
					ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0],error_code);
					RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,2);
					return Erase_Error;				
				}else{
					/* to do */
				}
			}
			/* Calculate Lengh of file and number of packet will receiver */
			number_package  = (ASCToHEXCHAR(data_receiver[4]) <<12) + (ASCToHEXCHAR(data_receiver[5]) <<8) + (ASCToHEXCHAR(data_receiver[6]) <<4) + ASCToHEXCHAR(data_receiver[7]);
			debug_len =sprintf((char *)&debug_arr[0],"Number package: %d!\n",number_package);
			debug_send((uint8_t *)&debug_arr[0],debug_len);
			if(number_package >= MAX_NUMBER_PACKAGE)
			{
				debug_len =sprintf((char *)&debug_arr[0],"Number package: exceed expect!\n");
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				/* timeout de thiet bi gui lai ban tin */
				return SMS_GET_FILE_EXCEPT;
			}
			/* Update number of package */
			if(update_size(number_package) < 0)
			{
				debug_len =sprintf((char *)&debug_arr[0],"Update size false!\n");
				debug_send((uint8_t *)&debug_arr[0],debug_len);
				error_code = 1;
				ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0],error_code);
				RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,2);
				return Write_Error;
			};
			/* Xoa bo check van hanh cua dieu hoa */
			clear_request_check_air();
			/* xoa so package da nhan */
			before_package = 0;
			error_code   = 0;
			/* bat co bao qua trinh update firmware bat dau */
			update_firmware_request = 1;
			/* Recheck */
			error_code = 0;
			/* tat dieu hoa truoc khi update */
			control_air(CMD_OFF_AIR,air_id);
			/* end */
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0],error_code);
			RS485_Respone((uint8_t *)&data_response[0],opcode,2);
		}
		break;
		case SMS_OPCODE_FIRMWARE_PACKET:
		{
			if(update_firmware_request)
			{	
				package_timeout = 0;
				/* Check size */
				if(len_data != LENG_SMS_PACKAGE_FIRMWARE)
				{	
					debug_len =sprintf((char *)&debug_arr[0],"Package wrong,aboart!\n");
					debug_send((uint8_t *)&debug_arr[0],debug_len);
					/* Sai ban tin, khong tra loi de main gui lai ban tin */
					return FIRMWARE_PACKAGE_LOST;
				}
				id_package = (ASCToHEXCHAR(data_receiver[4]) << 12) | (ASCToHEXCHAR(data_receiver[5]) << 8) | (ASCToHEXCHAR(data_receiver[6]) << 4) | ASCToHEXCHAR(data_receiver[7]);
				/* check id_package */
				if(id_package - before_package > 1) /* lost ban tin truoc mat bo qua case nay */
				{
					debug_len =sprintf((char *)&debug_arr[0],"Lost ban tin, before package: %d and now package: %d !\n",before_package,id_package);
					debug_send((uint8_t *)&debug_arr[0],debug_len);
					/* error code */
					error_code = LOST_FIRMWARE_PACKAGE;
					/* xoa yeu cau update firmware de module hoat dong binh thuong */
					update_firmware_request = 0;
					/*copy id ban tin */
					memcpy((uint8_t *)&data_response[0],&data_receiver[4],4);
					ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],error_code);
					RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,6);
					return FIRMWARE_UPDATE_PACKAGE_FALSE;				
				}else{
					/* ban tin dung, gan lai id ban tin */
					before_package = id_package;
				}
				/* Receiver data from package*/
				for( uint8_t i = 0;i <LENG_FIRMWARE; i++)
				{
					/* chuyen 128 bytes data tu ascii sang binary */
					firmware[i] = ASCToHEXCHAR(data_receiver[8 + 2* i]) * 16 + ASCToHEXCHAR(data_receiver[8 + (2*i +1)]); /* data from byte foure */
				}
				/* Write to external flash */
				if(save_firmware_external_flash(&firmware[0], id_package) < 0)
				{
					debug_len =sprintf((char *)&debug_arr[0],"Save firmware to flash false, package = %d!\n",id_package);
					debug_send((uint8_t *)&debug_arr[0],debug_len);
					/* error code */
					error_code = SAVE_STATUS_UPDATE_FAIL;
					/* xoa yeu cau update firmware de module hoat dong binh thuong */
					update_firmware_request = 0;
					/*copy id ban tin */
					memcpy((uint8_t *)&data_response[0],&data_receiver[4],4);
					ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],error_code);
					RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,6);
					return FIRMWARE_UPDATE_PACKAGE_FALSE;
				};
				/* If number of package is full */
				if(number_package == (id_package + 1))
				{
					debug_len =sprintf((char *)&debug_arr[0],"Du so ban tin can update! \n");
					debug_send((uint8_t *)&debug_arr[0],debug_len);
					 error_code = 0;
					/* Update request */
					 if(request_update_firmware() < 0)
					 {
						 debug_len =sprintf((char *)&debug_arr[0],"request update false!\n");
						 debug_send((uint8_t *)&debug_arr[0],debug_len);
						 error_code = SAVE_STATUS_UPDATE_FAIL;
						 update_firmware_request = 0;
						 memcpy((uint8_t *)&data_response[0],&data_receiver[4],4);
						 ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],error_code);
						 RS485_Respone((uint8_t *)&data_response[0],opcode + 0x80,6);
						 /* return false */
						 return FIRMWARE_UPDATE_PACKAGE_FALSE;
					 };
					 debug_len =sprintf((char *)&debug_arr[0],"request update success, resetting...!\n");
					 debug_send((uint8_t *)&debug_arr[0],debug_len);
					 error_code = 0;					 
					 memcpy((uint8_t *)&data_response[0],&data_receiver[4],4);
					 ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],error_code);
					 RS485_Respone((uint8_t *)&data_response[0],opcode,6);
					 /* Reboot system to update new firmware */
					 NVIC_SystemReset(); /* System reset to jump to bootloader */		
					 while(1);/* Wait reset */
				}
				/* For debug only */
				error_code = 0;
				memcpy((uint8_t *)&data_response[0],&data_receiver[4],4);
				ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[4],error_code);
				RS485_Respone((uint8_t *)&data_response[0],opcode,6);
			}else{
				/* to do */
				/* Khong co qua trinh update firmware neu truoc do khong co ban tin noty update firmware hop le */
			}
		}
		break;
		case SMS_OPCODE_FIRMWARE_CHECK:
		{
			len_data_send = 0;
			/* Byte dau tien la byte sate loi flash */
			data_response[len_data_send] = get_error();
			/* Hexa number from 0 to 9 */
			if (data_response[len_data_send] <= 9)  
			{
				data_response[len_data_send ++] += 0x30;
			}
			else  /* Hexa number from A to F */
			{
				data_response[len_data_send ++] += 0x37;
			}
			memcpy(&data_response[len_data_send],(uint8_t *)&software_version[0],strlen((char *)&software_version[0]));
			len_data_send += strlen((char *)&software_version[0]);
			data_response[len_data_send++] = ',';			
			memcpy(&data_response[len_data_send],(uint8_t *)&hardware_version[0],strlen((char *)&hardware_version[0]));
			len_data_send += strlen((char *)&hardware_version[0]);
			data_response[len_data_send++] = ',';
			/* Coppy serial number */
			memcpy(&data_response[len_data_send],&Serial_number[0],strlen((char *)&Serial_number[0]));
			len_data_send += strlen((char *)&Serial_number[0]);
			RS485_Respone((uint8_t *)&data_response,opcode,len_data_send);
		}
		break;
		case SMS_OPCODE_RESET_MODULE:
		{
			//debug_len =sprintf((char *)&debug_arr[0],"Ban tin reset module! \n");
			//debug_send((uint8_t *)&debug_arr[0],debug_len);
			error_code = 0;
			ConvertUINT8ToHEXCHAR((uint8_t *)&data_response[0],error_code);
			RS485_Respone((uint8_t *)&data_response[0],opcode,2);		
			/* reset */
			NVIC_SystemReset(); /* System reset to jump to bootloader */
		}
		break;
		default:
			break;
	}
	return 0;
}
/**
  * @brief This fuction get one byte one data from rs485
  * The first character (SOT) and final character(EOT) of one message are abort.
  * One message have struct : 
	---------------------------------------------
	| SOT | Address | Opcode | Data | CRC | EOT |
	---------------------------------------------
  * Call handle message function when received final character .
  * @param  none.
  * @retval none.
  */

void RS485_GetRequest(void)
{
	uint8_t c = 0;
	static uint8_t startOfMsg = 0;
	#define LEN_SHORTEST_SMS 6
	if (ringbuf_elements(&rxBuf)) {
		c = ringbuf_get(&rxBuf);
		if (c == RS485_SOH_CHAR) {
			getBufferLen = 0;
			startOfMsg = 1;
			return;
		}
	
		if (startOfMsg) {
			getBuffer[getBufferLen++] = c;
			if (getBufferLen >= RS485_MAX_RX_SIZE) {
				getBufferLen = 0;
				startOfMsg = 0;
				return;
			}
		
			if (c == RS485_EOT_CHAR /* && getBufferLen */ ) {
				if (getBufferLen > LEN_SHORTEST_SMS) {
					/* replace PROTO_EOT_CHAR by NULL */
					getBuffer[getBufferLen - 1] = 0;
					getBuffer[getBufferLen - 2] = 0;
					getBufferLen -= 2;
					/* call handler */
					RS485_InMessageHandler((char *)&getBuffer[0], getBufferLen);
				}
				/* end of msg */
				getBufferLen = 0;
				startOfMsg = 0;
			}
		}
	}
}


/*************************************************************************************************/

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
void    RS485_Respone(uint8_t *data,uint8_t opcode, uint8_t len)
{

	uint32_t bufLen				=  0;
	uint8_t crc_cal				=  0;  /* Calculate CRC */
	uint8_t end_respone[3]		= {0x0D,0x0A,0}; /* CRLF */
	char sms[150] 				={0};
	memset(sms,0,150);
	/* Address */
	sms[0] = RS485_SOH_CHAR;
	ConvertUINT8ToHEXCHAR((uint8_t *)&sms[1], id485_module);
	/* Data for opcode */
	ConvertUINT8ToHEXCHAR((uint8_t *)&sms[3], opcode);
	/* Caculate information for error*/
	for(uint8_t count = 0; count < len; count ++)
		sms[count +5] = data[count];
	bufLen = len + 5;	
	
	crc_cal = Cal_CRC8((uint8_t *)&sms[1], bufLen - 1);
	/* CRC sms error respone*/
	ConvertUINT8ToHEXCHAR((uint8_t *)&sms[bufLen], crc_cal);
	/* Message to send */
	bufLen += 2;
	sms[bufLen] = end_respone[0];
	sms[bufLen + 1] = end_respone[1];
	bufLen += 2;
	//debug_send((uint8_t *)sms,bufLen);
	RS485_Write((uint8_t *)&sms[0], bufLen);
}

void rs485_reinit(void)
{
	/* re-initialize rs485 */
	RS485_Init(); 	
	RS485_Listen();
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
			/* For debug Only */
void 	debug_send(uint8_t *Data, uint8_t len)
{
	if(HAL_UART_Transmit(&UartDebug, &Data[0], len, 0xFFFF) != HAL_OK)
		while(1);
}
void 	debug_putc(uint8_t Data)
{
	if(HAL_UART_Transmit(&UartDebug, &Data, 1, 0xF) != HAL_OK)
		while(1);
}

uint8_t get_mode_control_air(void)
{
	return air_mode_set;
}
uint8_t get_temp_set_air(void)
{
	return air_temp_set;
}
uint8_t get_fanlevel_air(void)
{
	return fan_level_set;
}
uint8_t get_swing_air(void)
{
	return swing_set;
}
void get_id485_module(void)
{
	id485_module = (HAL_GPIO_ReadPin(ID485_DETERMINE_PORT, ID485_DETERMINE_PIN) == GPIO_PIN_SET)?ADDRESS_AIRCON2_BOARD:ADDRESS_AIRCON1_BOARD;
}