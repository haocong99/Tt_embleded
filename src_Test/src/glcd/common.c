#include "common.h"

void delay_us(uint32_t time)
{    
   uint32_t i=0;  
	
   while(time--)
   {
      i=10;
      while(i--) ;    
   }
}

void delay_ms(uint32_t time)
{    
   uint32_t i=0;  
	
   while(time--)
   {
      i=12000;
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


