#ifndef __CRC_H
#define __CRC_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
#include <stdint.h>
	 
	 #define POLY16	0x1021
	 #define POLY32	0x04C11DB7
	 /*-----------------------------------------------------------------------------------------------*/
uint8_t  Cal_CRC8(uint8_t* data, uint32_t size);
uint8_t ds18b20crc8( uint8_t *data, uint8_t length );
uint8_t ow_crc( uint8_t x);
uint8_t crc_get(void);
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */
