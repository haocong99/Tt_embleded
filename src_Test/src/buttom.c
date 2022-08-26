/**
 ******************************************************************************
 * @file    /ctOSv1/include/leds.h
 * @author  Clever Things JSC.
 * @version 1.0
 * @date    Apr 10, 2016
 * @brief   This file contained the headers of  handlers
 * @history
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE IS PROPERTY OF CLEVERTHINGS JSC., ALL CUSTOMIZATION
 * IS NOT ALLOWED. AS A RESULT, CLEVERTHINGS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE MODIFIED CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS
 * OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2016 CleverThings JSC.</center></h2>
 ******************************************************************************
 */

#include "buttom.h"
#include "utils.h"
#include "stdint.h"
#include "stdbool.h"
#include "debug.h"
typedef struct{
	char characters;
	uint8_t count;
	uint8_t status;
}button_t;
button_t arr_button[5][5] = {{{'6',0,0},{'7',0,0},{'8',0,0},{'9',0,0},{'0',0,0}},	\
													   {{'1',0,0},{'2',0,0},{'3',0,0},{'4',0,0},{'5',0,0}},	\
													   {{'C',0,0},{'N',0,0},{'D',0,0},{'N',0,0},{'S',0,0}},	\
													   {{'N',0,0},{'L',0,0},{'O',0,0},{'R',0,0},{'N',0,0}},	\
												     {{'A',0,0},{'N',0,0},{'U',0,0},{'N',0,0},{'T',0,0}}};	

static char buttom_event = 'N';
char get_event_button(void)
{
	return buttom_event;
}
void clear_event_button(void)
{
	buttom_event = 'N';
}
void button_scan(void){
	static uint64_t last_time_scan = 0;
	uint8_t arr_row[5] 		= {0,0,0,0,0};
	uint8_t arr_collum[5] = {0,0,0,0,0};
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(get_sys_tick() - last_time_scan < 100){
		// Bo qua quet phim 
		return;
	}
	last_time_scan = get_sys_tick();
	
	// Khoi tao clock cac pin
	STML0_ROW1_CLK_ENABLE();
	STML0_ROW2_CLK_ENABLE();
	STML0_ROW3_CLK_ENABLE();
	STML0_ROW4_CLK_ENABLE();
	STML0_ROW5_CLK_ENABLE();
	
	STML0_COL1_CLK_ENABLE();	
	STML0_COL2_CLK_ENABLE();
	STML0_COL3_CLK_ENABLE();
	STML0_COL4_CLK_ENABLE();
	STML0_COL5_CLK_ENABLE();	

	memset(&arr_collum[0],0,5);
	memset(&arr_row[0],0,5);
	
	/* Kiem tra cot truoc toan bo hang set 1 */
	GPIO_InitStruct.Pin = STML0_ROW1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;		
  HAL_GPIO_Init(STML0_ROW1_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW2_Pin;
  HAL_GPIO_Init(STML0_ROW2_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW3_Pin;
  HAL_GPIO_Init(STML0_ROW3_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW4_Pin;
  HAL_GPIO_Init(STML0_ROW4_Port, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = STML0_ROW5_Pin;
  HAL_GPIO_Init(STML0_ROW5_Port, &GPIO_InitStruct);		

	HAL_GPIO_WritePin(STML0_ROW1_Port, STML0_ROW1_Pin, GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(STML0_ROW2_Port, STML0_ROW2_Pin, GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(STML0_ROW3_Port, STML0_ROW3_Pin, GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(STML0_ROW4_Port, STML0_ROW4_Pin, GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(STML0_ROW5_Port, STML0_ROW5_Pin, GPIO_PIN_RESET);		

	HAL_GPIO_WritePin(STML0_COL1_Port, STML0_COL1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STML0_COL2_Port, STML0_COL2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STML0_COL3_Port, STML0_COL3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STML0_COL4_Port, STML0_COL4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(STML0_COL5_Port, STML0_COL5_Pin, GPIO_PIN_SET);
	
	/* Cau hinh cot */
	GPIO_InitStruct.Pin = STML0_COL1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	
  HAL_GPIO_Init(STML0_COL1_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL2_Pin;
  HAL_GPIO_Init(STML0_COL2_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL3_Pin;
  HAL_GPIO_Init(STML0_COL3_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL4_Pin;
  HAL_GPIO_Init(STML0_COL4_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL5_Pin;
  HAL_GPIO_Init(STML0_COL5_Port, &GPIO_InitStruct);	

	
	/* delay truoc khi doc */
	HAL_Delay(2);
	
	/* Doc du lieu */
	arr_collum[0] = HAL_GPIO_ReadPin(STML0_COL1_Port, STML0_COL1_Pin)?0:1;
	arr_collum[1] = HAL_GPIO_ReadPin(STML0_COL2_Port, STML0_COL2_Pin)?0:1;
	arr_collum[2] = HAL_GPIO_ReadPin(STML0_COL3_Port, STML0_COL3_Pin)?0:1;
	arr_collum[3] = HAL_GPIO_ReadPin(STML0_COL4_Port, STML0_COL4_Pin)?0:1;
	arr_collum[4] = HAL_GPIO_ReadPin(STML0_COL5_Port, STML0_COL5_Pin)?0:1;

	//DBG_NPRINTF("Collum %d %d %d %d %d\n",arr_collum[0],arr_collum[1],arr_collum[2],arr_collum[3],arr_collum[4]);
	
	HAL_GPIO_WritePin(STML0_ROW1_Port, STML0_ROW1_Pin, GPIO_PIN_SET);	
	HAL_GPIO_WritePin(STML0_ROW2_Port, STML0_ROW2_Pin, GPIO_PIN_SET);	
	HAL_GPIO_WritePin(STML0_ROW3_Port, STML0_ROW3_Pin, GPIO_PIN_SET);	
	HAL_GPIO_WritePin(STML0_ROW4_Port, STML0_ROW4_Pin, GPIO_PIN_SET);	
	HAL_GPIO_WritePin(STML0_ROW5_Port, STML0_ROW5_Pin, GPIO_PIN_SET);	
	
	HAL_GPIO_WritePin(STML0_COL1_Port, STML0_COL1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STML0_COL2_Port, STML0_COL2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STML0_COL3_Port, STML0_COL3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STML0_COL4_Port, STML0_COL4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(STML0_COL5_Port, STML0_COL5_Pin, GPIO_PIN_RESET);	
	
	/* Dao hang va cot */
	/* Kiem tra cot truoc toan bo hang set 1 */
	GPIO_InitStruct.Pin = STML0_ROW1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;		
  HAL_GPIO_Init(STML0_ROW1_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW2_Pin;
  HAL_GPIO_Init(STML0_ROW2_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW3_Pin;
  HAL_GPIO_Init(STML0_ROW3_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_ROW4_Pin;
  HAL_GPIO_Init(STML0_ROW4_Port, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = STML0_ROW5_Pin;
  HAL_GPIO_Init(STML0_ROW5_Port, &GPIO_InitStruct);		
	
	/* Cau hinh cot */
	GPIO_InitStruct.Pin = STML0_COL1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	
  HAL_GPIO_Init(STML0_COL1_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL2_Pin;
  HAL_GPIO_Init(STML0_COL2_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL3_Pin;
  HAL_GPIO_Init(STML0_COL3_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL4_Pin;
  HAL_GPIO_Init(STML0_COL4_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = STML0_COL5_Pin;
  HAL_GPIO_Init(STML0_COL5_Port, &GPIO_InitStruct);		

	/* delay truoc khi doc */
	HAL_Delay(2);
	arr_row[0] = HAL_GPIO_ReadPin(STML0_ROW1_Port, STML0_ROW1_Pin)?0:1;
	arr_row[1] = HAL_GPIO_ReadPin(STML0_ROW2_Port, STML0_ROW2_Pin)?0:1;
	arr_row[2] = HAL_GPIO_ReadPin(STML0_ROW3_Port, STML0_ROW3_Pin)?0:1;
	arr_row[3] = HAL_GPIO_ReadPin(STML0_ROW4_Port, STML0_ROW4_Pin)?0:1;
	arr_row[4] = HAL_GPIO_ReadPin(STML0_ROW5_Port, STML0_ROW5_Pin)?0:1;

	/* Hien thi */
	extern char arr_debug[255];
	for(uint8_t i = 0; i < 5; i ++)
	{
		for(uint8_t j =0;j < 5; j ++){		
			if((arr_row[i] == 1) && (arr_collum[j] == 1)){	
				if(arr_button[i][j].status == 1){
					// Duoc bam truoc do, bo qua						
				}else{
					// Chua duoc bam truoc do 
					arr_button[i][j].count += 1;					
					if(arr_button[i][j].count >= FILTER_BUTTOM){
							// Xac nhan phim duoc bam 
							arr_button[i][j].status = 1;
							// Tao event
							if(arr_button[i][j].characters != 'N'){
								buttom_event = arr_button[i][j].characters;
								//set_button(buttom_event);
								sprintf(&arr_debug[0],"button %c duoc bam\n",buttom_event);
								rs485_PutByteArray(&arr_debug[0]);
							}								
					}else{
						// Khong lam gi ca
					}
				}
			}else{
				// Xoa trang thai, xoa bien dem	
				arr_button[i][j].status = 0;
				arr_button[i][j].count = 0;
			}
		}
	}
}
