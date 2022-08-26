#include "SHT31.h"
#include "std_header.h"
#include "debug.h"

/* Initialize I2C  */
I2C_HandleTypeDef hi2c1;
static const uint8_t crctable_temp[256] = { // Sensirion 8-bit CRC x8+x5+x4+1
    0x00,0x31,0x62,0x53,0xC4,0xF5,0xA6,0x97,  // 0x31 byte table - sourcer32@gmail.com
    0xB9,0x88,0xDB,0xEA,0x7D,0x4C,0x1F,0x2E,
    0x43,0x72,0x21,0x10,0x87,0xB6,0xE5,0xD4,
    0xFA,0xCB,0x98,0xA9,0x3E,0x0F,0x5C,0x6D,
    0x86,0xB7,0xE4,0xD5,0x42,0x73,0x20,0x11,
    0x3F,0x0E,0x5D,0x6C,0xFB,0xCA,0x99,0xA8,
    0xC5,0xF4,0xA7,0x96,0x01,0x30,0x63,0x52,
    0x7C,0x4D,0x1E,0x2F,0xB8,0x89,0xDA,0xEB,
    0x3D,0x0C,0x5F,0x6E,0xF9,0xC8,0x9B,0xAA,
    0x84,0xB5,0xE6,0xD7,0x40,0x71,0x22,0x13,
    0x7E,0x4F,0x1C,0x2D,0xBA,0x8B,0xD8,0xE9,
    0xC7,0xF6,0xA5,0x94,0x03,0x32,0x61,0x50,
    0xBB,0x8A,0xD9,0xE8,0x7F,0x4E,0x1D,0x2C,
    0x02,0x33,0x60,0x51,0xC6,0xF7,0xA4,0x95,
    0xF8,0xC9,0x9A,0xAB,0x3C,0x0D,0x5E,0x6F,
    0x41,0x70,0x23,0x12,0x85,0xB4,0xE7,0xD6,
    0x7A,0x4B,0x18,0x29,0xBE,0x8F,0xDC,0xED,
    0xC3,0xF2,0xA1,0x90,0x07,0x36,0x65,0x54,
    0x39,0x08,0x5B,0x6A,0xFD,0xCC,0x9F,0xAE,
    0x80,0xB1,0xE2,0xD3,0x44,0x75,0x26,0x17,
    0xFC,0xCD,0x9E,0xAF,0x38,0x09,0x5A,0x6B,
    0x45,0x74,0x27,0x16,0x81,0xB0,0xE3,0xD2,
    0xBF,0x8E,0xDD,0xEC,0x7B,0x4A,0x19,0x28,
    0x06,0x37,0x64,0x55,0xC2,0xF3,0xA0,0x91,
    0x47,0x76,0x25,0x14,0x83,0xB2,0xE1,0xD0,
    0xFE,0xCF,0x9C,0xAD,0x3A,0x0B,0x58,0x69,
    0x04,0x35,0x66,0x57,0xC0,0xF1,0xA2,0x93,
    0xBD,0x8C,0xDF,0xEE,0x79,0x48,0x1B,0x2A,
    0xC1,0xF0,0xA3,0x92,0x05,0x34,0x67,0x56,
    0x78,0x49,0x1A,0x2B,0xBC,0x8D,0xDE,0xEF,
    0x82,0xB3,0xE0,0xD1,0x46,0x77,0x24,0x15,
    0x3B,0x0A,0x59,0x68,0xFF,0xCE,0x9D,0xAC 
};

/* the poly is 0x31 : x^8 +x^5 +x^4 +1 */
uint8_t cal_crc8_temp(uint8_t * data, uint32_t size)
{
unsigned int crc=0xff;
 while(size--)
  {
    crc = crc ^ *(data++); // Apply data byte
    crc = crctable_temp[crc & 0xFF]; // Apply CRC 8-bits at a time
  }

  return(crc);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	/* I2C pins configuration */
	/* Enable the clock for the I2C GPIOs */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStructure.Pin   = IN_TEMP_HUMI_SS1_SDA_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(TEMP_HUMI_SS1_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.Pin   = IN_TEMP_HUMI_SS1_SCL_PIN;
	GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(TEMP_HUMI_SS1_PORT, &GPIO_InitStructure); 
	
	__HAL_RCC_I2C1_CLK_ENABLE();

	/* Configure I2C pins as alternate function open-drain */	
	
	GPIO_InitStructure.Pin   = RESET_SHT31_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(RESET_SHT31_PORT, &GPIO_InitStructure);  
	/* Enable power for sensor*/
	ENABLE_SHT31();
}
extern char arr_debug[255];
extern uint8_t len_debug;
void I2C_begin(void)
{
	/* Enable the I2Cx interface clock */
	hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = (uint32_t)0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0x01;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	//hi2c1.Mode = HAL_I2C_MODE_MASTER;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
		  len_debug = sprintf((char *)&arr_debug[0],"Init i2c fail: %d\n",HAL_I2C_GetState(&hi2c1));
			rs485_PutByteArray((char *)&arr_debug[0]);
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
		  len_debug = sprintf((char *)&arr_debug[0],"HAL_I2CEx_ConfigAnalogFilter fail: %d\n",HAL_I2C_GetState(&hi2c1));
			rs485_PutByteArray((char *)&arr_debug[0]);   
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
		  len_debug = sprintf((char *)&arr_debug[0],"HAL_I2CEx_ConfigDigitalFilter fail: %d\n",HAL_I2C_GetState(&hi2c1));
			rs485_PutByteArray((char *)&arr_debug[0]);     
  }
	// Gui lenh reset sensor
	soft_reset();
	len_debug = sprintf((char *)&arr_debug[0],"Init i2C done: %d\n",HAL_I2C_GetState(&hi2c1));
	rs485_PutByteArray((char *)&arr_debug[0]);   
}
/* Write command */
uint8_t writeCommand(uint16_t cmd)
{ 
	uint32_t current_time = HAL_GetTick();
	uint8_t arr_cmd[3] ={0};
	memset(arr_cmd,0,3);
	arr_cmd[0] = (cmd >> 8);
	arr_cmd[1] = cmd & 0xff;
	 /* Wait ready */
	while ((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) && (HAL_GetTick() - current_time < 300)){
  }
	if((HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)){
		rs485_PutByteArray((char *)&"fail 1\n");
		return HAL_ERROR;
	}
 while((HAL_I2C_Master_Transmit(&hi2c1,SHT31_DEFAULT_ADDR << 1 , &arr_cmd[0],2,1000)) != HAL_OK){
		if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF){
			rs485_PutByteArray((char *)&"fail 2\n");
			return HAL_ERROR;
		 };
		 len_debug = sprintf(&arr_debug[0],"fail 3, status : %d\n",HAL_I2C_GetError(&hi2c1));
		 rs485_PutByteArray((char *)&arr_debug[0]);
		return HAL_ERROR;
	};
	/*Wait until finish transmit */
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){
	}
	return HAL_OK;
 }
  
 /* reset */
void soft_reset(void)
{
		uint32_t now_time = 0;
	  writeCommand(SHT31_SOFTRESET);
	  now_time =  HAL_GetTick();
	  while( HAL_GetTick() - now_time <50);
 }
/* status */
  uint16_t readStatus(void) {
	uint8_t arr_status[3] ={0};
	memset(arr_status,0,3);
	uint16_t status;
	writeCommand(SHT31_READSTATUS);
	while(HAL_I2C_Master_Receive(&hi2c1,SHT31_DEFAULT_ADDR  << 1 ,&arr_status[0],3,1000) != HAL_OK){	
	  if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF){
		}
	};
	status = (arr_status[0] <<8) | arr_status[1];
	return status;
}
 
void heater(uint8_t heater)
{
	if (heater)
		writeCommand(SHT31_HEATEREN);
	else
		writeCommand(SHT31_HEATERDIS);
}
sht31_data_t sht31 = {0,0,0,0};
int8_t readTempHum()
{
	static uint64_t last_time_read_sensor = 0;	
	if(get_sys_tick() - last_time_read_sensor < 3000){
		
		return 0;
	}	
	uint32_t now_time = 0;
	uint8_t readbuffer[7];
	memset(readbuffer,0,7);
	last_time_read_sensor = get_sys_tick();
	if(writeCommand(SHT31_MEAS_HIGHREP) == HAL_ERROR){
		return -1;
	}
  now_time =  HAL_GetTick();
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY && (HAL_GetTick() - now_time < 300)){
  }
	/*check if false */
	if(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){
		return -2;
	}
	while(HAL_I2C_Master_Receive(&hi2c1,SHT31_DEFAULT_ADDR  << 1 ,&readbuffer[0],6,1000) !=HAL_OK){
		if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF){
			return -3;
		}
	}
	uint16_t ST, SRH;
	ST = readbuffer[0];
	ST <<= 8;
	ST |= readbuffer[1];

	if (readbuffer[2] != cal_crc8_temp(&readbuffer[0],2)){
		return -4;
	}

	SRH = readbuffer[3];
	SRH <<= 8;
	SRH |= readbuffer[4];

	if (readbuffer[5] != cal_crc8_temp(&readbuffer[3],2)) {
		return -5;
	}
	double stemp = ST;
	stemp *= 175;
	stemp /= 0xffff;
	stemp = -45 + stemp;
	sht31.temper = (int16_t)(stemp *10);

	double shum = SRH;
	shum *= 100;
	shum /= 0xFFFF;

	sht31.humi = (int16_t)(shum*10);
	return 0;
}
  void monitor_sensor(void)
	{
		static uint64_t last_time_read_sensor = 0;
		if(get_sys_tick() - last_time_read_sensor < 3000){
			return;
		}
		last_time_read_sensor = get_sys_tick();
		// doc sen sor 
		if(readTempHum() < 0){
			sht31.error = 1;
			sht31.count_error ++;
			if(sht31.count_error >= 10){
				reset_internal_sensor();
				sht31.count_error = 0;
			}else{
				// do nothing 
			}
		}else{
				sht31.count_error = 0;
				sht31.error = 0;
		}
	}
 void reset_internal_sensor(void)
  {
     
    hi2c1.Instance->CR1 |= I2C_CR1_PECEN;
		DISABLE_SHT31();
		HAL_Delay(100);
    hi2c1.Instance->CR1 &= ~I2C_CR1_PECEN;
    I2C_begin();
    HAL_Delay(200);
  }
  