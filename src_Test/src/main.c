#include "prj_config.h"
#include "std_header.h"
#include "hw.h"
#include "stm32f10x_it.h"
#include "remote.h"
#include "debug.h"
#include "sim908.h"
#include "evt_mng.h"
#include "utils.h"
#include "var.h"
#include "rtc.h"
#include "leds.h"
#include "adc.h"
#include "monitor.h"
#include "packet.h"
#include "sms_config.h"
#include "gps.h"
#include <math.h>

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

char packet[MAX_PACKET_SIZE];
typedef enum {
    DISABLE_SEND_DATA = 0,
    ENABLE_SEND_DATA
}data_mode_t;
data_mode_t data_mode = ENABLE_SEND_DATA;
uint32_t gps_fix = 0;
gps_data_t gps;
uint8_t ctime_enable_flag = 0;
int16_t temper = TEMPER_DEFAULT;
uint32_t current_led =0;
uint32_t gps_not_fix_cnt = 0;
uint32_t period_time_to_send_msg = 0;
uint8_t dev_id[7]= {0,0,1,0xFF,0xFF,0xFF,0xFF};
float gps_lat_coordinator = 0;
float gps_long_coordinator = 0;
uint8_t first_gps_position = 0;
uint32_t period_gps_check_time = 180;
uint8_t time_synced = 0;


int32_t check_position(float new_lat, float new_long, float coord_lat, float coord_long) {
	float delta_lat, delta_long;
	float distance;
	float radians = 0;
	radians = ((new_lat + coord_lat) / 2) * (3.14159265359 / 180);
	delta_lat = fabsf(new_lat - coord_lat) * 111194.9;
	delta_long = 111194.9 * fabsf(new_long - coord_long) * cos(radians);
	distance = sqrt(pow(delta_lat, 2) + pow(delta_long, 2));
	printf("Khoang cach dich chuyen: %3.2f/%u m\n", distance, param.distance);
	if (distance > (param.distance + 10)) {
		return -1;
	}
	return 0;
}




void evt_send_msg_to_srv(void) {
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)	
    //set_signal(SIGNAL_PERIOD_GPS_CHECK);
    set_signal(SIGNAL_SEND_MSG_TO_SRV);
#endif
}

void gsm_choose_pwr_startup(uint32_t vbat)
{
	/* if Vbat > xxx -> switch to pwr1 (current flow through diode ) */
	/* else if Vbat < xxx -> switch to pwr 2 */ 
	//if (vbat >= 4300) {
		GSM_PWR1_ON();
		//GSM_PWR2_OFF();
	//} else {
	//	GSM_PWR2_ON();
	//	GSM_PWR1_OFF();
	//}
}

void gsm_choose_pwr_run_time(uint32_t vbat)
{
	/* if Vbat > xxx -> switch to pwr1 (current flow through diode ) */
	/* else if Vbat < xxx -> switch to pwr 2 */ 
	//if (vbat >= 4300) {
		GSM_PWR1_ON();
	//	GSM_PWR2_OFF();
	//} else if( vbat < 4200) {
	//	GSM_PWR2_ON();
	//	GSM_PWR1_OFF();
	//} else {
		/* do nothing */
	//}
	
	
}

uint8_t get_next_ctime_msg(void){
	uint8_t next_send_period_msg = 0;
	
	/* get next time send period message */
	uint32_t current_tim_insec = RTC_GetTimeInSec();
	// find first message to send	
	next_send_period_msg = 0;
	// update send flag in case of new 
	while (next_send_period_msg < TIME_SIZE && current_tim_insec > param.next_period_send_msg[next_send_period_msg]*60
		&& (param.next_period_send_msg[next_send_period_msg] != 0xFFFF)){
		next_send_period_msg++;
	}
//	printf("time synced: %d, current time: %d, next first time send: %d\r\n", time_synced, current_tim_insec, param.next_period_send_msg[0]*60);
	if(param.next_period_send_msg[next_send_period_msg] == 0xFFFF || next_send_period_msg == TIME_SIZE){
		next_send_period_msg=0;
		ctime_enable_flag = 2; // finish sending, wait to next day or new SMS
		// update as all messages have been sent
		// All CTIME messages have been sent or CTIME is not configured.
	} else if(param.next_period_send_msg[next_send_period_msg] != 0xFFFF && next_send_period_msg < TIME_SIZE)
		ctime_enable_flag = 1;
	return next_send_period_msg;
}
char test[] = "092222222*1a31,2114,2116,2018";
int main(void)
{
    RUNNING_MODE mode = INIT_MODE;
    uint32_t time_to_check_at = 0;
    uint8_t gsm_signal = 0;
    uint8_t i = 0;
    uint32_t current_sum = 0;
    struct tm tim;
    uint8_t gsm_ret = 0;
    uint8_t gps_fix_count = 0;
    uint16_t msg_len = 0;
    int get_gps_ret = 0;
    char gps_buf[100];
    uint32_t signal = 0;   
    uint32_t highestPriorEvt = SIGNAL_SWITCH_RUN_MODE;
    uint32_t last_time_gps_not_fix = 0;
    uint32_t time_to_send_msg_period = 0;
    uint32_t time_to_check_gps = 0;
	uint32_t time_to_check_adc = 0;
    float old_gps_lat = 0;
    float old_gps_long = 0;
    uint8_t retry_send_data = 0;
    m1_battery_level_state_t m1_battery_level = 0;
    m2_battery_level_state_t m2_battery_level = 0;
	uint32_t time_start_night_mode = 0;
	uint32_t current_tim_insec = 0;
	uint8_t last_send_period_msg = 0;
	uint8_t send_period_msg_flag[TIME_SIZE];
	uint8_t gps_fix_ctime = 0;
	uint8_t gps_fix_change_mode = 0;
	uint8_t gps_fix_ngam = 0;
	uint32_t batt_voltage = 0;
	uint8_t phao_moving = 0;
	uint16_t period_adc_check = 1;
    if (SysTick_Config(SystemCoreClock/sys_time_freq)) /* tick 10us */
    { 
        /* Capture error */ 
        while (1);
    }
    //* Read protect the flash.  NEVER EVER set this to level 2.  You can't
    // * write to the chip ever again after that. */
//.    FLASH_ReadOutProtection (ENABLE);
	rcc_init();
	debug_init();
    remote_init();
	IWDG_Configuration();
    InitLEDs();	
    TEST_ADC1_Init();
	RTC_Init();		
		
#if 0
	PWR_IR_ON();
	LED_START_PWR();
	//LED_STATUS1_ON();
	recv_cmd_ir(30);
	LED_SHUTDOWN_PWR();	
	PWR_IR_OFF();
#endif
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)	
	batt_voltage = TEST_get_voltage(BAT_VOLTAGE_ADC_CHANNEL, BAT_POWER_FACTOR);
	gsm_choose_pwr_startup(batt_voltage);
	
	/* start hardware */
    sim908_init();
	
	GSM_PowerOn();
#endif
#if defined (VERSION_GSM_GPS)
    gps_init();
#endif
	
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)	
	/* config gsm gps */
    Gsm_Start();
	
#endif
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, DISABLE);
//	//GPS_Standby_Mode();
//	//SIM908_Sleep();
//	while(1)
//	{
//		Gsm_SendCmd("AT+CSQ\r", 1000, "OK", 2);
//		//__wfi();
//		IWDG_ReloadCounter();
//		delay_10ms(1000);
//	}		
#if defined (VERSION_GSM_GPS)
    gps_config();
#endif
		
      
    if( init_param() < 0) {
        NVIC_SystemReset();
    } else {
		memcpy(current_lat,param.lat,10);
		memcpy(current_lng,param.lng,11);
	}
    time_to_check_at = get_system_time_s();
	time_to_check_adc = get_system_time_10ms();
	time_to_check_gps = get_system_time_s();
   // time_to_send_msg_period = get_system_time_s();
    period_gps_check_time = 1800;
	set_signal(SIGNAL_PERIOD_GPS_CHECK);
	set_signal(SIGNAL_SEND_MSG_TO_SRV);
	
    while(1) {
		IWDG_ReloadCounter();
		if (get_device_op_mode() != DEEP_SAVING_MODE) {
			LED_Check();
		}
		/* run in night mode > 18h -> go to deep sleep */
		if ( time_start_night_mode && get_system_time_s()- time_start_night_mode > 64800) {
			if (get_device_op_mode() != DEEP_SAVING_MODE) {
				set_device_op_mode(DEEP_SAVING_MODE);
				set_signal(SIGNAL_OP_MODE_CHANGE);
			}
		}
		/* monitor battery, solar, led periodically */
		if (get_system_time_s() - time_to_check_adc >= period_adc_check) {
			set_signal(SIGNAL_COMMON_TASK);
			time_to_check_adc = get_system_time_s();
			tim = RTC_GetCalendar();
			//Gsm_SendCmd("AT+CSQ\r", 1000, "OK", 2);
#if defined (DEBUG_MAIN)											
			DBG_PRINTF("Time: %02d:%02d:%02d\n", tim.tm_hour, tim.tm_min, tim.tm_sec);
#endif
		}
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)
		gsm_choose_pwr_run_time(GetBatteryVoltage());		
		if (get_system_time_s() - time_to_check_at >= 300 && get_device_op_mode() == NORMAL_MODE) {
			Gsm_SendCmd("AT+CSQ\r", 1000, "OK", 2);
			if (Gsm_SendCmd("AT\r", 1000, "OK", 2) == CMD_OK) { 			
#if defined (DEBUG_MAIN)								
			   DBG_PRINTF("check at period\r\n");
#endif			
			   if (Gsm_SendCmd("AT+CPIN?\r", 3000, "+CPIN: READY", 1) == CMD_OK) {
#if defined (DEBUG_MAIN)									
				  DBG_PRINTF("check sim ok\r\n");
#endif
			   } else {
#if defined (DEBUG_MAIN)								
				  DBG_PRINTF("check sim fail\r\n");    
#endif				   
				  SoftResetSIMCOM();
				  continue;
			   }           
			   time_to_check_at = get_system_time_s();
			} else {
			   GSM_PWR1_OFF();
			   GSM_PWR2_OFF();
				delay_10ms(50);
			    gsm_choose_pwr_run_time(GetBatteryVoltage());
			   GSM_PowerOn();
				Gsm_Start();
			   continue;
			}
			CheckSignalQuality(1, &gsm_signal);	   
		}
#endif
		
#if defined (VERSION_GSM_GPS)
		if (get_device_op_mode() == NORMAL_MODE) {
			if (get_system_time_s() - time_to_check_gps > period_gps_check_time && gps_fix_ngam == 0) {
				set_signal(SIGNAL_PERIOD_GPS_CHECK);
				time_to_check_gps = get_system_time_s();		
			}
			if(gps_fix_ngam == 1) {
				if((cfg_get_gps_check_type()==1 || cfg_get_gps_check_type()==2 || cfg_get_gps_check_type()==3) 
					&&(get_system_time_s() - time_to_check_gps > cfg_get_next_gps_check())){//
					set_signal(SIGNAL_PERIOD_GPS_CHECK);
					time_to_check_gps = get_system_time_s();
					DBG_PRINTF("Fixing GPS periodically");
				}
			}
		}		
#endif
		
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)
		if(time_synced >= 1) {
			current_tim_insec = RTC_GetTimeInSec();
			if(current_tim_insec < param.next_period_send_msg[0]*60
					&& param.next_period_send_msg[0] != 0xFFFF
					&& (ctime_enable_flag == 0 || ctime_enable_flag ==2))			
			{
				printf("Reset all send flags Ctime\r\n");
				last_send_period_msg = 0;
				ctime_enable_flag = 1;
				/* update all send flags after last time of day */
				for(i=0; i<TIME_SIZE; i++){
					send_period_msg_flag[i] = 0;
					printf(" %d ", param.next_period_send_msg[i]);
				}
			}
			else if(ctime_enable_flag == 0){
				last_send_period_msg = get_next_ctime_msg();
				for(i=0;i<TIME_SIZE;i++)
					send_period_msg_flag[i]=0;
			}
		}	
#endif		
		signal = get_signal();
	   //Khong co viec gi de lam thi ngoi quet adc
	   if (signal == 0) {
		   //continue;
		   set_signal(SIGNAL_IDLE_TSK);
		   continue;
	   }
	   //tim su kien co uu tien cao nhat
	   highestPriorEvt = SIGNAL_NEW_SMS;
	   /*--------------Nen lock doan xu ly code nay lai----------*/
	   while((signal & highestPriorEvt)== 0)
	   {
			highestPriorEvt <<= 1;
	   }
	   //reset Evt
	   clear_signal(highestPriorEvt);
	   switch(highestPriorEvt) {
			case SIGNAL_COMMON_TASK:
				SystemMonitor();	
				if(GetOSEvent(E_LED_ERROR)) {
					set_signal(SIGNAL_LED_ERROR);
#if defined (DEBUG_MAIN)
					DBG_PRINTF("LEDs error\r\n");
#endif
				}
				if(GetOSEvent(E_BAT_ERROR)) {
					set_signal(SIGNAL_BAT_ERROR);
#if defined (DEBUG_MAIN)
					DBG_PRINTF("BATT error\r\n");
#endif
				}
				if(GetOSEvent(E_LED_NORMAL)) {
					#if 0
					led_measure_current();        
					#endif
					set_signal(SIGNAL_LED_NORMAL);
#if defined (DEBUG_MAIN)				
					DBG_PRINTF("LEDs normal\r\n");
#endif
				}
				if(GetOSEvent(E_OPERATION_MODE_CHANGE)) {
					set_signal(SIGNAL_OP_MODE_CHANGE);
#if defined (DEBUG_MAIN)				
					DBG_PRINTF("operation mode change\r\n");
#endif
				}         
				if(GetOSEvent(E_NIGHT_DAY_CHANGE)){
#if defined (DEBUG_MAIN)				
					DBG_PRINTF("Working mode changed from Night to Day mode\r\n");
#endif
					period_adc_check = 10; 
					set_signal(SIGNAL_WAIT_REMOTE);
					set_signal(SIGNAL_SWITCH_RUN_MODE);
				}
				if(GetOSEvent(E_DAY_NIGHT_CHANGE)) {
					#if 0
					led_measure_current();
					#endif
#if defined (DEBUG_MAIN)				
					DBG_PRINTF("Working mode changed from Day to Night mode\r\n");
#endif
					period_adc_check = 10; 
					set_signal(SIGNAL_WAIT_REMOTE);
					set_signal(SIGNAL_SWITCH_RUN_MODE);
				}
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)

				/* Check periodic time to send message after time synced */
				if (time_synced >= 1 && ctime_enable_flag == 1 && get_device_op_mode() == NORMAL_MODE) {				
					if(current_tim_insec >= param.next_period_send_msg[last_send_period_msg]*60 && send_period_msg_flag[last_send_period_msg] == 0){
						gps_fix_ctime = 1;
#if defined (DEBUG_MAIN)										
						printf("\r\n-> CTIME %.2u:%.2u\n", 
							param.next_period_send_msg[last_send_period_msg]/60, param.next_period_send_msg[last_send_period_msg]%60);
#endif
						msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
						evt_send_msg_to_srv();
						/* Update send flag */
						send_period_msg_flag[last_send_period_msg] = 1;
						last_send_period_msg++;
						if((last_send_period_msg==TIME_SIZE) || (param.next_period_send_msg[last_send_period_msg] == 0xFFFF)){
							last_send_period_msg = 0; // finish sending, wait to next day or new sms
							ctime_enable_flag = 2;
#if defined (DEBUG_MAIN)											
							printf("All CTIME messages have been sent");
#endif
						}
					}
				}
#endif
			   break;
		   case SIGNAL_SWITCH_RUN_MODE:
			   // khi chuyen tu ngay sang dem
				//cho phep chay den, cho phep gui ban tin, cho phep nhan remote
				if(get_current_mode() == NIGHT_MODE) {
	                DBG_PRINTF("start pwm for night mode\r\n");
	 //               pwm_start();
					//TIM_Cmd(PWM_TIMER, ENABLE);
					//reinit
					//InitializePWMChannel(0.8);
					InitLEDs();
					led_set_pwm_mode(LED_PWM_CTL);
					led_pwm_start();
					time_start_night_mode = get_system_time_s();
				}           
				// khi chuyen tu dem sang ngay
				// tat den, cho phep gui ban tin, cho phep nhan remote, tat flash
				if ((get_current_mode() == DAY_MODE) || (get_current_mode() == INIT_MODE)) {
#if defined (DEBUG_MAIN)
					DBG_PRINTF("stop pwm\r\n");
#endif
					//TIM_Cmd(PWM_TIMER, DISABLE);
					led_pwm_stop();
					led_set_pwm_mode(LED_DO_CTL);
					LED_SIGNAL_OFF();
					time_start_night_mode = 0;
				}
				//SIM908_Wakeup();
				//GPS_WakeUp();
			   break;
			case SIGNAL_CTL_LED_CHANGE_STATE:
				if (cfg_get_led_control_mode() == LED_CONTROL_MODE_1) {
					led_set_duty(M2_CAP_LEVEL_1);
				} 
				else if (cfg_get_led_control_mode() == LED_CONTROL_MODE_2) { 
					led_set_duty(M2_CAP_LEVEL_2);
				} 
				else if (cfg_get_led_control_mode() == LED_CONTROL_MODE_3) { 
					led_set_duty(M2_CAP_LEVEL_3);
				}
			   break;
			case SIGNAL_LED_NORMAL:
				evt_send_msg_to_srv();   
			   break;
			case SIGNAL_LED_ERROR:
			   msg_len = creat_led_message(packet);
				evt_send_msg_to_srv();             
			   break;
			case SIGNAL_BAT_NORMAL:
				//evt_send_msg_to_srv();   
			   break;
			case SIGNAL_BAT_ERROR:
			    msg_len = creat_bat_message(packet);       
				evt_send_msg_to_srv();            
			   break;
			case SIGNAL_SEND_MSG_TO_SRV:  
				IWDG_ReloadCounter();
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)
			
				if (get_device_op_mode() != NORMAL_MODE) {
					break;
				}
				/* wake up module sim */
				
				/* check at command, cpin */
				
				/* TODO:package message here or somewhere */
				
				/* check if we need send msg via SMS or GPRS */
				
				/* send SMS */
				
				/* send GPRS */
				
				/* force module sim go to sleep if message is sent */
			 #if 1
				//when gsm sleep, use sys_time_10ms time base
			    sys_time_freq=100;
				sys_time_10_ms = 1;
				if (SysTick_Config(SystemCoreClock/sys_time_freq))
				{ 
					/* Capture error */ 
					while (1);
				}
			   SIM908_Wakeup();
				Gsm_SendCmd("AT\r",1000,"OK",3);
				Gsm_SendCmd("AT\r",1000,"OK",3);
				delay_10ms(200);
				
				/* send msg */
			   //gsm_ret = SIM_SendDataViaGprsNoRecv(cfg_get_backup_domain(), (uint16_t)cfg_get_backup_port(), packet, msg_len, 1);
			   /* send msg */
			   gsm_ret = SIM_SendDataViaGprsNoRecv(cfg_get_host_domain(), (uint16_t)cfg_get_host_port(), packet, msg_len, 1);
			   if (gsm_ret != SEND_OK) {
				   
				   retry_send_data ++;
				   // neu gps da ngu thi khong gui tiep nua, tranh trung hop cu gui mai khi khong co mang
	//               if (!GPS_GetSleepStatus()) {
	//                    set_signal(SIGNAL_SEND_MSG_TO_SRV);  
	//                } 
					if (retry_send_data < 3) {
#if defined (DEBUG_MAIN)
					DBG_PRINTF("SEnd GPRS fail, retry\r\n");
#endif						
						set_signal(SIGNAL_SEND_MSG_TO_SRV);
					} else {                   
						retry_send_data = 0;
#if defined (DEBUG_MAIN)
					DBG_PRINTF("SEnd GPRS fail, send SMS\r\n");
#endif						
						Gsm_SendMsg(packet, param.phone1, 2);
					}
					if (gsm_ret == CMD_ERROR || gsm_ret == PDP_DEACT) {
#if defined (DEBUG_MAIN)
					DBG_PRINTF("Softreset gsm\r\n");
#endif							
						IWDG_ReloadCounter();
						SoftResetSIMCOM();
						IWDG_ReloadCounter();                    
						Gsm_SendCmd("AT+CIPSHUT\r",2000,"SHUT OK",1);
						//Gsm_SendCmd("AT+CGATT=1\r",2000,"OK",1);                    
					}
			   } else {
				   retry_send_data = 0;
#if defined (DEBUG_MAIN)
					DBG_PRINTF("SEnd GPRS Done\r\n");
#endif				
			   }
			   // sim908 ngu tiep
			   SIM908_Sleep();
//			   //when gsm sleep, don't care sys_time_10ms time base
//			   sys_time_freq=10;
//				sys_time_10_ms = 1;
//				if (SysTick_Config(SystemCoreClock/sys_time_freq))
//				{ 
//					/* Capture error */ 
//					while (1);
//				}
			   #endif
#endif
			   break;
		   case SIGNAL_WAIT_REMOTE:
#if defined (DEBUG_MAIN)
				DBG_PRINTF("\r\n wait ir\r\n");
#endif
				led_measure_current();
				sys_time_freq=10000;
				sys_time_10_ms = 100;	
				if (SysTick_Config(SystemCoreClock/sys_time_freq))
				{ 
					/* Capture error */ 
					while (1);
				}
				NVIC_EnableIRQ(EXTI_DI_IR_IRQn);
				PWR_IR_ON();
//				led_blinks(2);
				recv_cmd_ir(30);  
				PWR_IR_OFF();
				set_signal(SIGNAL_CTL_LED_CHANGE_STATE);
				gps_fix_change_mode = 1;
				msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
				NVIC_DisableIRQ(EXTI_DI_IR_IRQn);
				//evt_send_msg_to_srv();
				set_signal(SIGNAL_PERIOD_GPS_CHECK);
				sys_time_freq=100;
				sys_time_10_ms = 1;
				if (SysTick_Config(SystemCoreClock/sys_time_freq))
				{ 
					/* Capture error */ 
					while (1);
				}
			    break;
		   case SIGNAL_NEW_SMS:
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)
#if defined (DEBUG_MAIN)
			   DBG_PRINTF("Have a message\r\n");
#endif
				//when gsm wakeup, use sys_time_10ms time base
			    sys_time_freq=100;
				sys_time_10_ms = 1;
				if (SysTick_Config(SystemCoreClock/sys_time_freq))
				{ 
					/* Capture error */ 
					while (1);
				}
			   SIM908_Wakeup();
			   if (!process_new_sms()) {
					//evt_send_msg_to_srv(); 
			   }    
			   SIM908_Sleep();
			   //when gsm sleep, don't care sys_time_10ms time base
//			   sys_time_freq=10;
//				sys_time_10_ms = 1;
//				if (SysTick_Config(SystemCoreClock/sys_time_freq))
//				{ 
//					/* Capture error */ 
//					while (1);
//				}
#endif			   
			   break;
		   case SIGNAL_OP_MODE_CHANGE:
				if (get_device_op_mode() == SAVING_MODE) {
					printf (" saving mode ");
					if (led_get_running_state() == OFF) {
						led_enable();
					}
					/* set led duty 20% */
					led_set_duty(0.2);               
					/* turn off gsm, gps */
#if defined VERSION_GSM_GPS || VERSION_GSM
					GSM_PWR1_OFF();
					GSM_PWR2_OFF();
#endif					
#if defined VERSION_GSM_GPS
					GPS_PWR_OFF();
#endif					
				} else if (get_device_op_mode() == DEEP_SAVING_MODE) {
					printf ("deep saving mode ");
					/* disable led first */
					led_disable();
					led_pwm_stop();
					/* TODO: disable peripheral, timer if we need */
					/* turn off gsm, gps */
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)
					GSM_PWR1_OFF();
					GSM_PWR2_OFF();
#endif					
#if defined (VERSION_GSM_GPS)
					GPS_PWR_OFF();
#endif						                                   
				} else {
					/* normal mode */
					printf ("normal mode ");
					/* disable first to ensure gsm module is turned off */
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)					
					GSM_PWR1_OFF();
					GSM_PWR2_OFF();
					delay_10ms(20);
					/* turn on gsm and gps power */
					gsm_choose_pwr_startup(GetBatteryVoltage());
					GSM_PowerOn();
					/* reinit gsm */
					Gsm_Start();
					
#endif	
					
#if defined (VERSION_GSM_GPS)					
					GPS_PWR_ON();
					/* re-init gps */
					gps_init();
					gps_config();
#endif
					/* led enable */
					if (led_get_running_state() == OFF) {
						led_enable();
					}
					//GSM_PWR_ON();
					//GSM_PowerOn();
					//Gsm_Start();
					//GPS_WakeUp();
					
					//
				}
#if defined (VERSION_GSM_GPS) || (VERSION_GSM)				
				msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
				evt_send_msg_to_srv();
#endif
			   break;
		   case SIGNAL_PERIOD_GPS_CHECK:
				/* disable period gps check if dev isn't in normal mode */
#if defined (VERSION_GSM_GPS)
				if (get_device_op_mode() != NORMAL_MODE) {
					break;
				}
				 /* wake up gps */
				 
				 GPS_WakeUp();  

				/* get data, timeout 2 minute */
				get_gps_ret = get_gps(&gps);
				if  (get_gps_ret == 1) {
#if defined (DEBUG_MAIN)					
					DBG_PRINTF("\r\ngps not fix\r\n");       
#endif					
					if (get_system_time_s() - last_time_gps_not_fix < 45) {
						/* request check again */
						set_signal(SIGNAL_PERIOD_GPS_CHECK);
						//clear mutex to continue recv data
						//gps_mutex = 0;
						break;
					}
					last_time_gps_not_fix = get_system_time_s();             
					gps_not_fix_cnt ++;
					/* 2 *60 = 120s timeout */
					if (gps_not_fix_cnt > 2) {  
						/* sleep gps when timeout */
						GPS_Standby_Mode();
						/* chi gui khi chuyen mode */
						if (gps_fix_change_mode) {
							msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
							evt_send_msg_to_srv();
						}
						gps_not_fix_cnt = 0;
						sms_gps_pos = 0;
						gps_fix_change_mode = 0;
						gps_fix_ctime = 0;
						gps_fix_ngam = 0;
					} else {
						set_signal(SIGNAL_PERIOD_GPS_CHECK);                        
					}
				} else if (get_gps_ret == 0) {
					gps_not_fix_cnt = 0;
					/* synchronous if time is different */
					tim = RTC_GetCalendar();
					/* chi can check ngay va thang, vi khi gps da fix duoc thi ngay, thang, nam se deu dung */
					if ((tim.tm_min != gps.minute) || (tim.tm_hour != gps.hours) || (tim.tm_mday != gps.date) || (tim.tm_mon != gps.month)) {  
#if defined (DEBUG_MAIN)						
					DBG_PRINTF("Synchronous time by gps\n");
#endif
						tim.tm_hour = gps.hours;
						tim.tm_min = gps.minute;
						tim.tm_sec = gps.second;
						tim.tm_mday = gps.date;
						tim.tm_mon = gps.month;
						tim.tm_year = gps.year + 2000;
						RTC_SetCalendarTime(tim);
						
					}
					time_synced = 1;
					gps_fix_count ++;
					#if 0
					if (gps_fix_count >= 10) {
						/* sleep gps */
						GPS_Standby_Mode();
						gps_fix_count = 0;
						DBG_PRINTF("\r\ngps fix, go to sleep\r\n");   
					} else {
					/* request check position again if don't have enough 10 times */
						set_signal(SIGNAL_PERIOD_GPS_CHECK);
					}
					#endif
					if(sms_gps_pos == 1 && gps_fix_count==30) {
						 cfg_set_root_location(rx_gps_buf_copy); // save root GPS localtion
						 snprintf(current_lat, 10, "%f", convertToDecimalFormat(gps.latitude));
						 snprintf(current_lng, 11, "%f", convertToDecimalFormat(gps.longitude));
						 sms_gps_pos = 0;
						 gps_fix_count=0;
						 gps_fix_ngam = 0;
#if defined (DEBUG_MAIN)
						 printf("\r\nSAVE GPS: %s, %s\r\n", current_lat, current_lng);
#endif
						 msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
						 evt_send_msg_to_srv();
						 break;
					 }
					 else if(gps_fix_change_mode == 1 && gps_fix_count == 30){
						 snprintf(current_lat, 10, "%f", convertToDecimalFormat(gps.latitude));
						 snprintf(current_lng, 11, "%f", convertToDecimalFormat(gps.longitude));
						 gps_fix_count = 0;
						 gps_fix_change_mode = 0;
#if defined (DEBUG_MAIN)
						 printf("\r\GPS: Change mode: %s, %s\r\n", current_lat, current_lng);
#endif						 
						 msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
						 evt_send_msg_to_srv();
						 break;
					 }
					 else if(gps_fix_ctime == 1 && gps_fix_count == 30){
						 snprintf(current_lat, 10, "%f", convertToDecimalFormat(gps.latitude));
						 snprintf(current_lng, 11, "%f", convertToDecimalFormat(gps.longitude));
						 gps_fix_count = 0;
						 gps_fix_ctime = 0;
#if defined (DEBUG_MAIN)
						 printf("\r\GPS: ctime: %s, %s\r\n", current_lat, current_lng);
#endif						 
						 msg_len = creat_periodic_message(packet, GetBatteryVoltage(), cfg_get_flashcode());
						 evt_send_msg_to_srv();
						 break;
					 }
					 else if (gps_fix_count == 10 && sms_gps_pos==0 && gps_fix_change_mode == 0 && gps_fix_ctime == 0) {
						 /* sleep gps when fix 10 times */
						/* sleep gps */
						GPS_Standby_Mode();
						gps_fix_count = 0;
#if defined (DEBUG_MAIN)						 
						DBG_PRINTF("\r\ngps fix, go to sleep\r\n");  
#endif
					 } 
					 else{
						 /* request check position again if don't have enough 10 times */
						set_signal(SIGNAL_PERIOD_GPS_CHECK);
					 }
				} else {
					set_signal(SIGNAL_PERIOD_GPS_CHECK);
				}                    
				/* check position of device when gps fixed, send msg to server if necessary */
				if (!get_gps_ret && gps_fix_count == 0) {
					old_gps_lat = convert_gps_data(param.lat);
					old_gps_long = convert_gps_data(param.lng);
					
					/*check position, if "phao" is moving, send msg to server*/
					if (check_position(gps.latitude, gps.longitude, old_gps_lat, old_gps_long)) {	
						phao_moving++;
						if(phao_moving >= 15){		
#if defined (DEBUG_MAIN)						 
							
							DBG_PRINTF("ALARM: OUT OF DISTANCE\r\n");
#endif
							snprintf(current_lat, 10, "%f", convertToDecimalFormat(gps.latitude));
							snprintf(current_lng, 11, "%f", convertToDecimalFormat(gps.longitude));
							/* log position to check again */
							old_gps_lat = gps.latitude;
							old_gps_long = gps.longitude;   
							gps_fix_ngam = 1;
							phao_moving = 0;
							set_signal(SIGNAL_SEND_MSG_TO_SRV);				
							msg_len = creat_moving_message(packet, rx_gps_buf_copy);
							//reset timer
							time_to_check_gps = get_system_time_s();
						} else {
							set_signal(SIGNAL_PERIOD_GPS_CHECK);
						}
							
					} else {					
						//reset timer
						time_to_check_gps = get_system_time_s();
						gps_fix_ngam = 0;
						 phao_moving = 0;
					}                     
			}
				/* release mutex to continue to receive data */
				//gps_mutex = 0;  
#endif				
				break;
		   case SIGNAL_IDLE_TSK:
			   
			   if (get_current_mode() == DAY_MODE) {
				   TIM_ITConfig(PWM_TIMER, TIM_IT_Update, DISABLE);
				   TIM_Cmd(PWM_TIMER, DISABLE);
				   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, DISABLE);
				   USART_DeInit(USART3);
				   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, DISABLE);
				   //PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
			   } else if (get_current_mode() == NIGHT_MODE) {
			   }
				__wfi();	
				debug_init();
			   break;
   }
 }
}


PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(PC_USART, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(PC_USART, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

void __aeabi_assert(const char * info, const char * file, int line)
{
    return;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
