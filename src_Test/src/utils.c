#include "utils.h"

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
uint8_t Digit2Dec(uint8_t c)
{
    if(c > 0x40)
        return (c-0x37);
    else
        return (c-0x30);

}
/* @brief: Convert char array to float
 * @agru: data and lenght of array
 * ex: data[3] = {'1','.','3'} -> 1.3(float)
 *
 */
float String2Float(char *str, char length)
{
    long integer_part = 0;
    float decimal_part = 0.0;
    float decimal_pivot = 0.1;
    uint8_t isdecimal = 0, isnegative = 0;
    uint8_t i = 0;
    uint8_t temp = 0;
    for(i= 0; i <length; i++)
    {
        temp = str[i];
        if(temp == '-')
        {
                isnegative = 1;
                continue;
        }
        if(temp == '.')
        {
                isdecimal = 1;
                continue;
        }
        if(temp == '+')
                continue;
        if (!isdecimal)
                integer_part = (10 * integer_part) + (temp - 48);		//temp - 48 de chuyen tu ki tu sang so thap phan
        else
        {
                decimal_part += decimal_pivot * (float)(temp - 48);
                decimal_pivot /= 10.0;
        }
    }
    // add integer part
    decimal_part += (float)integer_part;
    // check negative
    if (isnegative)  decimal_part = - decimal_part;

    return decimal_part;
}
uint8_t my_sprintf(char *buf,char *format,...)
{
  uint8_t len = 0;
  va_list args;
  va_start (args, format);
  len = vsprintf(buf, format, args);
  va_end(args);
  return len;
}

void delay_us(uint32_t time)
{    
   uint32_t i=0;  
	
   while(time--)
   {
		  i=5;
      while(i--) ;    
   }
}

void delay_ms(uint32_t time)
{    
   uint32_t i=0;  
	
   while(time--)
   {
      i=1000;
      while(i--) ;    
   }
}

void  set_bit( uint8_t *array,  uint16_t k )
{
    array[k/8] |= 1 << (k%8);  // Set the bit at the k-th position in A[i]
}

void  clear_bit( uint8_t *array,  uint16_t k )                
{
    array[k/8] &= ~(1 << (k%8));
}

uint8_t get_bit( uint8_t *array,  uint16_t k )
{
    return ( (array[k/8] & (1 << (k%8) ))?1:0) ;     
}

