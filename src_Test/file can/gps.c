#include <stdio.h>
#include <string.h>
#include "math.h"
#include "gps.h"
#include "utils.h"
#include "debug.h"
#include "hw.h"
uint8_t volatile gps_mutex = 0;
uint8_t length = 0;
char rx_gps_buf_copy[GPS_PACKET];
static uint8_t isFirstChar = 0;
static uint8_t count = 0;
static uint8_t rx_gps_buf[GPS_PACKET];
static uint8_t gps_sleep = 0;
uint8_t byte_gps_receiver = 0;
void gps_deinit(void);
UART_HandleTypeDef USART_GPS;

static void gps_put_string(char *str) {
	uint16_t len = strlen(str);
	HAL_UART_Transmit(&USART_GPS, (uint8_t *)str, len,100);
}

static void gps_put_byte_array(char *arr, uint16_t len) {
    uint16_t i = 0;
    for (i = 0; i < len; i++) {
		HAL_UART_Transmit(&USART_GPS, (uint8_t*)&arr[i], 1,10);
    }
}
void usart_gps_listen(void)
{
	HAL_UART_Receive_IT(&USART_GPS, &byte_gps_receiver, 1);
}
uint8_t gps_getchar(void)
{
	return byte_gps_receiver;
}
//version SIM28
void gps_init(void)
{	
  /* Enable USARTx clock */
  USARTx_GPS_CLK_ENABLE();
  
  USART_GPS.Instance        = GPS_USART;
  USART_GPS.Init.BaudRate   = GPS_USART_BAUDRATE;
  USART_GPS.Init.WordLength = UART_WORDLENGTH_8B;
  USART_GPS.Init.StopBits   = UART_STOPBITS_1;
  USART_GPS.Init.Parity     = UART_PARITY_NONE;
  USART_GPS.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  USART_GPS.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&USART_GPS) != HAL_OK)
  {
    //while (1);
  }  
  if(HAL_UART_Init(&USART_GPS) != HAL_OK)
  {
    // while (1);
  } 
  GPS_PWR_ON();
  usart_gps_listen();
}

void gps_deinit(void)
{
	 __HAL_RCC_USART2_CLK_DISABLE();
}
static uint8_t CalCheckSum(char *buf, uint8_t length)
{
	uint8_t i = 0;
	uint8_t result = 0;
	for(i = 1; i < (length - DATA_END_FIELD_SIZE - END_FIELD_SIZE - CHECK_SUM_FIELD_SIZE); i++)
		result ^= buf[i];
	return result;
}
static void EncodeCheckSum(uint8_t checksum, char *high, char *low)
{

	*high = (checksum & 0xF0) >> 4;
	*low = checksum & 0x0F;
	if( *high < 0x0A)
		*high = *high + 0x30;
	else
		*high = *high + 0x37;
	if( *low < 0x0A)
		*low = *low + 0x30;
	else
		*low = *low + 0x37;
	
}
static uint8_t DecodeCheckSum(uint8_t high, uint8_t low)
{
	if(high > 0x40)
		high = high -0x37;
	else
		high = high - 0x30;
	if(low > 0x40)
		low = low - 0x37;
	else
		low = low-0x30;
	return ((high << 4)+low); // ket qua tra ve khac voi return (high <<4+low)
}

void gps_process_receiver_data(uint8_t byte)
{
	uint8_t temp = byte;
	uint8_t i = 0;
	if(temp == START_CHAR)
	{
		isFirstChar = 1;
		count = 0;
	}
	if(isFirstChar == 1)
	{
		rx_gps_buf[count] = temp;
		if(count >= GPS_PACKET-1)
			count = 0;
		else
			count ++;
		if(temp == LF_CHAR && count != 0)
		{
			rx_gps_buf[count] = 0;
			isFirstChar = 0;
			/* filter GPRMC */
			
			// kiem tra xem da thao tac voi du lieu trong buf_copy xong chua
			if(rx_gps_buf[1] == 'G' && rx_gps_buf[2] == 'P' && 
				rx_gps_buf[3] == 'R'&& rx_gps_buf[4] == 'M'&& rx_gps_buf[5] == 'C') {
			
				if (!gps_mutex) {
				   
					//copy
					for(i = 0; i < count; i++)
						rx_gps_buf_copy[i] = rx_gps_buf[i];
					length = count;
					gps_mutex = 1;
				}
			}
		}	
	}
}
