#include "platform.h"
#include "var.h"
#include "common.h"
#define APPLICATION_ADDRESS	0x08000000
//		pFunction Jump_To_Application;
//		uint32_t JumpAddress;
uint32_t delay_time = 0;
uint8_t Ticks = 0;
// time(ms)
//uint8_t IsTick(uint32_t time)
//{
//	
//}
void Delay(uint32_t time)
{
	delay_time = time;
	//delay_time-- nam trong exception cua SysTick Timer
	while(delay_time);
}
void Int2String(char *buf,uint8_t *length, uint32_t dataIn)
{
	uint8_t i = 0;
	uint8_t count = 0;
	uint32_t data= dataIn;
	/* Dem so ki tu trong so int */
	do
	{
		
		data = data / 10;
		count++;
	}while(data > 0);
	/* set lai gia tri cho data nhu ban dau */
	data = dataIn;
	for(i = count ; i >0; i--)
	{
		buf[i-1] = (data %10) + 0x30;
		data = data/10;
	}
	*length = count;
}
static uint32_t myPow(uint32_t a, uint32_t b)
{
	uint32_t i = 0;
	uint32_t result;
	result = 1;
	for(i = 0; i < b; i++)
	{
		result *= a;
	}
	return result;
}
uint32_t String2Int(char *buf, uint8_t buf_len)
{
	uint8_t count_digit = 0;
	uint8_t i = 0;
	uint32_t result = 0;
	char *ptr;
	ptr = buf;
	while(*ptr++ != 0)
	{
		count_digit++;
	}
	for(i = 0; i < count_digit; i ++)
	{
		result += ((buf[i]-0x30) * myPow(10,count_digit - i -1));
	}
	return result;
}
//uint8_t CalCheckSum(uint8_t *buf, uint8_t length)
//{
//	uint8_t i = 0;
//	uint8_t result = 0;
//	for(i = 0; i < length;i++)
//		result ^= buf[i];
//	return result;
//		
//}
//void EncodeCheckSum(uint8_t checksum, uint8_t *high, uint8_t *low)
//{

//	*high = (checksum & 0xF0) >> 4;
//	*low = checksum & 0x0F;
//	if( *high < 0x0A)
//		*high = *high + 0x30;
//	else
//		*high = *high + 0x37;
//	if( *low < 0x0A)
//		*low = *low + 0x30;
//	else
//		*low = *low + 0x37;
//	
//}
// uint8_t DecodeCheckSum(uint8_t high, uint8_t low)
//{
//	if(high > 0x40)
//		high = high -0x37;
//	else
//		high = high - 0x30;
//	if(low > 0x40)
//		low = low - 0x37;
//	else
//		low = low-0x30;
//	return ((high << 4)+low); // ket qua tra ve khac voi return (high <<4+low)
//}

void Lock(uint8_t *task)
{
	*task = LOCK;
}
void UnLock(uint8_t *task)
{
	*task = UNLOCK;
}	
Lock_Status IsLock(uint8_t *task)
{
	if(*task == LOCK)
		return LOCKED;
	else
		return UNLOCKED;
}


