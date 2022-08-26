#include "debug.h"
UART_HandleTypeDef USART_Debug;
UART_HandleTypeDef USART_Rs485;
uint8_t byte_debug_receiver = 0;
uint8_t byte_rs485_receiver = 0;
status_pack_t status_data;
data_receiver uart_data;

void usart_debug_listen(void)
{
	HAL_UART_Receive_IT(&USART_Debug, &byte_debug_receiver, 1);
}
uint8_t debug_getchar(void)
{
	return byte_debug_receiver;
}

void usart_rs485_listen(){
	HAL_UART_Receive_IT(&USART_Rs485, &byte_rs485_receiver, 1);
}

uint8_t rs485_getchar(void)
{
	return byte_rs485_receiver;
}
/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA  
  *         used for USART data transmission     
  */
void USARTx_Debug_IRQHandler(void)
{
  HAL_UART_IRQHandler(&USART_Debug);
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&USART_Rs485);
}
//uint8_t com_buf[MAX_LEN_PACK];
void debug_init(void)
{
  USART_InitTypeDef USART_InitStructure;
	
  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();
  
  USART_Debug.Instance        = PC_USART;
	USART_Debug.Init.BaudRate   = PC_USART_BAUDRATE;
  USART_Debug.Init.WordLength = UART_WORDLENGTH_8B;
  USART_Debug.Init.StopBits   = UART_STOPBITS_1;
  USART_Debug.Init.Parity     = UART_PARITY_NONE;
  USART_Debug.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  USART_Debug.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&USART_Debug) != HAL_OK)
  {
    while (1);
  }  
  if(HAL_UART_Init(&USART_Debug) != HAL_OK)
  {
    while (1);
  } 
	//ringbuf_init(&g_ComRingBuf, &com_buf[0], MAX_LEN_PACK);	
	memset(&uart_data.data[0],0x00,MAX_LEN_PACK);
	uart_data.len = 0;
	uart_data.status_sms = NO_MSG;
	usart_debug_listen();
}
void debug_deinit(void)
{
	__HAL_RCC_USART1_CLK_DISABLE();
	HAL_UART_DeInit(&USART_Debug);
}
void UART_PutByte(uint8_t byte) {
	HAL_UART_Transmit(&USART_Debug, &byte, 1,2);
}
void UART_PutByteArray(char *arr, uint16_t len) {
	HAL_UART_Transmit(&USART_Debug, (uint8_t *)arr, len,100);
}

void rs485_init(void)
{
  USART_InitTypeDef USART_InitStructure;
	
  /* Enable USARTx clock */
  RS485_CLK_ENABLE();
  
  USART_Rs485.Instance        = RS485_USART;
	USART_Rs485.Init.BaudRate   = RS485_BAUDRATE;
  USART_Rs485.Init.WordLength = UART_WORDLENGTH_8B;
  USART_Rs485.Init.StopBits   = UART_STOPBITS_1;
  USART_Rs485.Init.Parity     = UART_PARITY_NONE;
  USART_Rs485.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  USART_Rs485.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&USART_Rs485) != HAL_OK)
  {
    while (1);
  }  
  if(HAL_UART_Init(&USART_Rs485) != HAL_OK)
  {
    while (1);
  }   
	RS485_READ_ENABLE();
	//usart_rs485_listen();	
}
void rs485_PutByte(uint8_t byte)
{
	RS485_WRITE_ENABLE();
	//HAL_Delay(1);
	HAL_UART_Transmit(&USART_Rs485, &byte, 1,2);
	RS485_READ_ENABLE();
}
void rs485_PutByteArray(char *arr)
{
	uint16_t len = strlen(arr);
	RS485_WRITE_ENABLE();
	HAL_UART_Transmit(&USART_Rs485, (uint8_t *)arr, len,len * 3);
	RS485_READ_ENABLE();
}

int8_t parse_pack(uint8_t *package, uint16_t len){
	int count;
	int start_pos=1;
	uint8_t buff[30];
	uint8_t buff_time[30];
	uint8_t buff_ID[30];
	uint8_t buff_CRC[5];
	uint8_t buff_response[30];
	memset((uint8_t*)&buff,0,30);
	count=start_pos;
	while(count<strlen((uint8_t*)&package[0])&&package[count]!=','){
		buff[count-start_pos]=package[count];
		count++;
		}
	start_pos=++count;
	memcpy((uint8_t*)&buff_ID[0],(uint8_t*)&buff[0],strlen((uint8_t*)&buff[0]));
	rs485_PutByteArray(&buff_ID[0]);
		HAL_Delay(10);
	memset((char*)&buff[0],0,30);
	while(count<strlen((char*)&package[0])&&package[count]!=','){
		buff[count-start_pos]=package[count];
		count++;
	}
	start_pos=++count;
	memset((char*)&buff_time[0],0,30);
	memcpy((char*)&buff_time[0],(char*)&buff[0],strlen((char*)&buff[0]));
	memset((char*)&buff[0],0,30);
	rs485_PutByteArray(&buff_time[0]);
	HAL_Delay(10);
	if(strlen((char *)&buff[start_pos])>2){
		memcpy(&buff[0],(char*)&package[start_pos],strlen((char *)&package[start_pos]));
	}
	memcpy((char*)&buff_CRC[0],(char*)&buff[0],strlen((char*)&buff[0]));
	memset((char*)&buff[0],0,30);
	rs485_PutByteArray(&buff_CRC[0]);
	HAL_Delay(10);
	sprintf(&buff_response[0],":%s,00,%s\r\n",&buff_ID[0],&buff_CRC[0]);
	rs485_PutByteArray(&buff_response[0]);
	return 0;
}

void proccess_msg(void) {
	uint16_t getBufferLen = 0;
	uint8_t startOfMsg = 0;
	uint8_t data[MAX_LEN_PACK];
	uint16_t len = 0;

	if(uart_data.status_sms == HAVE_MSG){
		/* Coppy du lieu */
		char a[30];
		memset((char*)&a[0],0,30);
		sprintf((char*)&a[0],"have_msg \r\n");
		UART_PutByteArray(&a[0],strlen((char*)&a[0]));
//		for(uint16_t i = 0; i < uart_data.len; i ++){
//			if (uart_data.data[i] == ':') {
//				getBufferLen = 0;
//				startOfMsg = 1;
//			}

//			if (startOfMsg) {
//				data[len++] = uart_data.data[i];
//				//getBufferLen++;
//				if (getBufferLen >= MAX_LEN_PACK) {
//					getBufferLen = 0;
//					startOfMsg = 0;
//					return;
//				}
//			if (uart_data.data[i] == 0x0A) {
//				if (len > MIN_LEN_PACK) {
//					/* replace PROTO_EOT_CHAR by NULL */
//					data[len - 1] = 0;
//					data[len - 2] = 0;
//					len -= 2;
//					/* call handler */
//					//parse_pack((uint8_t *)&data[1], len);
//					rs485_PutByteArray(&data[0]);
//				 HAL_Delay(10);
//				}				
//				/* end of msg */
//				len = 0;
//				startOfMsg = 0;
//		}
//		}
//	}		
		/* Tra lai buffer */
		uart_data.status_sms = NO_MSG;
		uart_data.len = 0;		
		memset(uart_data.data, 0, MAX_LEN_PACK);
		/* Pare ban tin o day ....*/
				
	}
}

