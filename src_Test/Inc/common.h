#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif
	 /*----------------------------------------------------------------------------------------------*/
	#include "misc.h"
	 
	 #define LOCK 	0
	 #define UNLOCK	1
	 typedef enum 
	 {
			LOCKED,
			UNLOCKED,
		}Lock_Status;
		extern uint32_t delay_time;
typedef  void (*pFunction)(void);
	//typedef  void (*pFunction)(void);
		//extern void USART_SendString(USART_TypeDef* usart,char* data);
	 //extern uint32_t myPow(uint32_t a, uint32_t b);
	 //extern uint8_t Char2Hex(uint8_t high, uint8_t low);
		extern void Delay(uint32_t time);
		//extern void USART_SendByteArray(USART_TypeDef* usart,uint8_t *data, uint32_t length);
		extern void Int2String(char *buf,uint8_t *length, uint32_t data);
		extern uint32_t String2Int(char *buf, uint8_t buf_len);
		extern void Lock(uint8_t *task);
		extern void UnLock(uint8_t *task);
		extern Lock_Status IsLock(uint8_t *task);
		// extern uint8_t DecodeCheckSum(uint8_t high, uint8_t low);
	//	extern void EncodeCheckSum(uint8_t checksum, uint8_t *high, uint8_t *low);
		//extern uint8_t CalCheckSum(uint8_t *buf, uint8_t length);
		extern void SDC_SELECT(void);
		extern void SDC_UNSELECT(void);
		extern void SDC_SELECT(void);
		extern void SDC_UNSELECT(void);
		void Reset_Chip(void);
	 /*-----------------------------------------------------------------------------------------------*/

	 
#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

