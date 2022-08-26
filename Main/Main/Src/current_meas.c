#include "current_meas.h"
#include "rs485.h"
#include "math.h"
#include "do.h"
#define AIRCON_ADC_CHANNEL    2
/* ADC handle declaration */
ADC_HandleTypeDef             AdcHandle;

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig;

/* Converted value declaration */
uint32_t          aResultDMA[AIRCON_ADC_CHANNEL] = {0,0};
uint32_t		  ADC_Count						= 0;

double 		  CUR_AIR1_VALUE = 0;
double 		  CUR_AIR2_VALUE = 0;

Current_t		  IRMS;
uint8_t			  ADC_ON_FLAG	= 0;
uint8_t 		  ADC_Finish	= 0;
uint8_t 		  Calib_Flag	 = 1; /* start calib when mcu wake up */
uint8_t			  Count_Calib	 = 0;
uint16_t	 	  Calib_Sum_Air1 = 0;
uint16_t		  Calib_Sum_Air2 = 0;
extern char	 							   Debug_Arr[50];
extern uint8_t								   debug_len;

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef           GPIO_InitStruct;
	static DMA_HandleTypeDef         DmaHandle;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock ****************************************/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/* ADC1 Periph clock enable */
	__HAL_RCC_ADC1_CLK_ENABLE();
	/* Enable DMA1 clock */
	__HAL_RCC_DMA1_CLK_ENABLE();


	/*##- 2- Configure peripheral GPIO #########################################*/
	/* ADC1 Channel8 GPIO pin configuration */
	GPIO_InitStruct.Pin = AIN_AIRCON1_PIN; /* Bug Hardware */
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(AIN_AIRCON1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = AIN_AIRCON2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(AIN_AIRCON2_PORT, &GPIO_InitStruct);

	/*##- 3- Configure DMA #####################################################*/ 

	/*********************** Configure DMA parameters ***************************/
	DmaHandle.Instance                 = DMA1_Channel1; // for ADc
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY; // peripheral to memory
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE; 
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	DmaHandle.Init.Mode                = DMA_CIRCULAR;
	DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;

	/* Deinitialize  & Initialize the DMA for new transfer */
	HAL_DMA_DeInit(&DmaHandle);  
	HAL_DMA_Init(&DmaHandle);

	/* Associate the DMA handle */
	__HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

	/* NVIC configuration for DMA Input data interrupt */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}


void ADC_Initialize(void)
{
	 /*__HAL_RCC_ADC1_CLK_ENABLE(); */
	/* ### - 1 - Initialize ADC peripheral #################################### */
	/*
	*  Instance                  = ADC1.
	*  ClockPrescaler            = PCLK divided by 4.
	*  LowPowerAutoWait          = Disabled
	*  LowPowerAutoPowerOff      = Disabled
	*  Resolution                = 12 bit (increased to 16 bit with oversampler)
	*  ScanConvMode              = ADC_SCAN_ENABLE 
	*  DataAlign                 = Right
	*  ContinuousConvMode        = Enabled
	*  DiscontinuousConvMode     = Enabled
	*  ExternalTrigConv          = ADC_SOFTWARE_START
	*  ExternalTrigConvEdge      = None (Software start)
	*  EOCSelection              = End Of Conversion event
	*  DMAContinuousRequests     = ENABLE
	*/

	AdcHandle.Instance = ADC1;

	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;
	AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.ContinuousConvMode    = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.EOCSelection          = ADC_EOC_SEQ_CONV;
	AdcHandle.Init.DMAContinuousRequests = ENABLE;
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;

	/* Initialize ADC peripheral according to the passed parameters */
	if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
	{
		while(1);
	}


	/* ### - 2 - Start calibration ############################################ */
	if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
	{
		while(1);
	}


	/* ### - 3 - Channel configuration ######################################## */
	/* Select Channel 0 to be converted */
	sConfig.Channel      = AIN_AIRCON1_CURRENT_CHANNEL;/* PA1 : Bug hardware changed */
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		while(1);
	}
	sConfig.Channel      = AIN_AIRCON2_CURRENT_CHANNEL; 
	sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
	{
		while(1);
	}

//	/* ### - 4 - Start conversion in DMA mode ################################# */
//	if (HAL_ADC_Start_DMA(&AdcHandle, aResultDMA, 2) != HAL_OK)
//	{
//		while(1);
//	} 

}
/**
  * @brief  Restart convert, Interrupt after conversion complete
  * @param  None
  * @retval None
  */
void ADC_Restart(void)
{
	if (HAL_ADC_Start_DMA(&AdcHandle, aResultDMA, 2) != HAL_OK)
	{
		while(1);
	}  
}
/**
  * @brief  Stop convert, interrupt ADC
  * @param  None
  * @retval None
  */
void ADC_DeInitialize(void)
{
		if ( HAL_ADC_Stop_DMA(&AdcHandle))
		{
			while(1);
		} 	
}
/**
  * @brief  Interrupt ADC handling 
  * @param  None
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *HAL_ADC_ConvCpltCallback)
{
	uint16_t adc1, adc2;
	adc1 = aResultDMA[1];
	adc2 = aResultDMA[0];
	/* Update result */
	if(ADC_Count < NUM_CONVERT_WITHIN80MS)
	{
		CUR_AIR1_VALUE	+= (adc1 * GAIN_CONVERT - Calib_Sum_Air1) * (adc1 * GAIN_CONVERT - Calib_Sum_Air1);
		CUR_AIR2_VALUE  += (adc2 * GAIN_CONVERT - Calib_Sum_Air2) * (adc2 * GAIN_CONVERT - Calib_Sum_Air2);
		ADC_Count ++;
	}
	else
	{
		ADC_DeInitialize();
		ADC_Finish = 1; /* finish convert */
	}
}

/**
  * @brief  This function calculate RMS current. 
  * @param  None
  * @retval None
  */
void Caculate_Current(void)
{
	char arr_debug[100] ={0};
	uint16_t len = 0;
	memset(arr_debug,0,100);
	
	uint16_t RMS_AIR1,RMS_AIR2;
	/* Calculate current */
	RMS_AIR1 = sqrt(CUR_AIR1_VALUE/NUM_CONVERT_WITHIN80MS)/SENSORTIVE *1000;
	RMS_AIR2 = sqrt(CUR_AIR2_VALUE/NUM_CONVERT_WITHIN80MS)/SENSORTIVE *1000;
//	len = sprintf(arr_debug,"ADC1  = %lu, ADC2  = %lu \n",(unsigned long long)CUR_AIR1_VALUE,(unsigned long long)CUR_AIR2_VALUE);
//	UART_Send((uint8_t *)&arr_debug[0],len);
	/* Compare and decise */
	if(RMS_AIR1 > 300) /* lager than 0.3 A */
	{
		IRMS.Current_Air1 = RMS_AIR1;
	}
	else
		IRMS.Current_Air1 = 0;
	
	if(RMS_AIR2 > 300) /* lager than 0.3 A */
	{
		IRMS.Current_Air2 = RMS_AIR2;
	}
	else
		IRMS.Current_Air2 = 0;
	
	CUR_AIR1_VALUE = 0;
	CUR_AIR2_VALUE = 0;
	ADC_Count = 0;
	/* Debug only */
//	len = sprintf(arr_debug,"I1 = %d, I2 = %d \n",IRMS.Current_Air1,IRMS.Current_Air2);
//	UART_Send((uint8_t *)&arr_debug[0],len);
}
/**
  * @brief  This function return value IRMSS current. 
  * @param  None
  * @retval float IRMS1
  */
uint16_t AIRCON_1_GetCurrent(void)
{
	return IRMS.Current_Air1;
}
/**
  * @brief  This function return value IRMSS current. 
  * @param  None
  * @retval float IRMS2
  */
uint16_t AIRCON_2_GetCurrent(void)
{
	return IRMS.Current_Air2;
   
}
void Calib_Current(void)
{
	ADC_Finish = 0;
	Calib_Sum_Air1 = sqrt(CUR_AIR1_VALUE/NUM_CONVERT_WITHIN80MS);
	Calib_Sum_Air2 = sqrt(CUR_AIR2_VALUE/NUM_CONVERT_WITHIN80MS);
	IRMS.Current_Air1 = 0;
	IRMS.Current_Air2 = 0;
	debug_len = sprintf(Debug_Arr,"calib1 = %d, calib1 = %d \n",Calib_Sum_Air1,Calib_Sum_Air2);
	UART_Send((uint8_t *)&Debug_Arr[0],debug_len);
}

void Disable_Interruppt()
{
	debug_len	= sprintf(Debug_Arr,"Disable_Interruppt !\n");
	UART_Send((uint8_t *)&Debug_Arr[0],debug_len);
	/* Disable interrupt timer*/
//	Deinitialize_Timer();
	/* Disable ADC */
	ADC_DeInitialize();
}

void calib_current_air1(void)
{
	Calib_Sum_Air1 = sqrt(CUR_AIR1_VALUE/NUM_CONVERT_WITHIN80MS);
}

void calib_current_air2(void)
{
	Calib_Sum_Air2 = sqrt(CUR_AIR2_VALUE/NUM_CONVERT_WITHIN80MS);
}