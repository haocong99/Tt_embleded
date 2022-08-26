#ifndef __GPS_H
#define __GPS_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include "std_header.h"
#include "prj_config.h"


/* define hardware */
#define GPS_USART_BAUDRATE 				9600
#define GPS_USART						USART2
#define GPS_USART_GPIO					GPIOA
#define USARTx_GPS_CLK_ENABLE()			__HAL_RCC_USART2_CLK_ENABLE()
	 
#define GPS_USART_TX					GPIO_PIN_3	
#define GPS_USART_RX					GPIO_PIN_2
     
#define GPS_TX_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()
#define GPS_RX_GPIO_CLK_ENABLE() 		__HAL_RCC_GPIOA_CLK_ENABLE()

/* Definition for USARTx's NVIC */
#define USARTx_GPS_IRQn                   USART2_IRQn
#define USARTx_GPS_IRQHandler             USART2_IRQHandler	 
     
#define TIMEZONE	            7
#define GPS_PACKET	            100				//max cua 1 message la 100 ki tu: co the dieu chinh tang them
#define START_CHAR	            0x24
#define DATA_END_CHAR	        0x2A
#define CR_CHAR	                0x0D
#define LF_CHAR                 0x0A
	 
#define DATA_END_FIELD_SIZE	    1
#define END_FIELD_SIZE          2
#define CHECK_SUM_FIELD_SIZE    2

#define NMEA_MESSAGE_ID_BASE    1	 
#define NMEA_DATA_FILED_BASE    6
	 
#define PMTK_TALKER_ID_BASE	    1
#define PMTK_PACKET_TYPE_BASE   5
#define PMTK_DATA_FIELD_BASE    8

#define CHAR_TIMEOUT	        0xFFFFF
#define SENTENCE_TIMEOUT	    0xFFFFFF
	 

/* Agurment for set_nmea_ouput*/
#define DISABLE_SENTENCE    0x30
#define ONE_FIX             0x31
#define TWO_FIX             0x32
#define THREE_FIX           0x33
#define FOUR_FIX            0x34
#define FIVE_FIX            0x35

typedef struct
{
	uint8_t hours;
	uint8_t minute;
	uint8_t second;
	uint8_t date;
	uint8_t month;
	uint8_t year;	
	float latitude;
	float longitude;
	float speed;
	uint8_t NorS;
	uint8_t EorW;
}gps_data_t;

//extern uint8_t gps_allow_recv;
extern uint32_t gps_timeOut;
//extern uint8_t rx_gps_buf[GPS_PACKET];
extern char rx_gps_buf_copy[GPS_PACKET];
extern uint8_t length;
void gps_init(void);
void usart_gps_listen(void);
uint8_t gps_getchar(void);
void gps_process_receiver_data(uint8_t byte);
int32_t gps_parse_data(gps_data_t *info);
     /*-----------------------------------------------------------------------------------------------*/

	 
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */


