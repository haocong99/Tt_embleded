#ifndef __UTILS_H
#define __UTILS_H

#include "std_header.h"
void delay_10us(uint32_t time);
void delay_10ms(uint32_t time);
void Int2String(char *buf,uint8_t *length, uint32_t dataIn);
uint8_t Digit2Dec(uint8_t c);
float String2Float(char *str, char length);
uint8_t my_sprintf(char *buf,char *format,...);
uint32_t String2Int(char *buf, uint8_t buf_len);
int32_t diff_time(uint32_t dst_h, uint32_t dst_m, uint32_t src_h, uint32_t src_m);

void delay_us(uint32_t time);
void delay_ms(uint32_t time);

void    set_bit( uint8_t *array,  uint16_t k );
void    clear_bit( uint8_t *array,  uint16_t k );
uint8_t get_bit( uint8_t *array,  uint16_t k );
#endif
