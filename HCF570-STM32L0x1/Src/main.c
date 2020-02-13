/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

uint8_t alarm_flag=0;
uint8_t wakeup_flag=0;


/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : HAL_Get_CPU_RCC_Clock
* Description    : ͨ�����ڴ�ӡʱ����Ϣ
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
void HAL_Get_CPU_RCC_Clock(void)
{
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_SysClockFreq:%fMHz\r\n",1.0*HAL_RCC_GetSysClockFreq()/1000000);
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_HCLKFreq:%fMHz\r\n",1.0*HAL_RCC_GetHCLKFreq()/1000000);
}

/*******************************************************************************
* Function Name  : Dev_ReInit
* Description    : �豸�����ʼ��
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
void Dev_ReInit(void)
{
	SystemClock_Config_MSI_2MHz();		/* Configure the system clock */
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_ADC_Init();
#ifdef debug_log
	MX_LPUART1_UART_Init();
#endif
	MX_RTC_Init();
	Set_Alarm();
	AT45dbxx_Init();
}

void Dev_parameter_init(void)
{
	uint8_t temp[2]={0},i=0;
	
	//��ɲ�����ʼ��
	EEWrite(0x00,temp,2);
	EERead(0x00,temp,2);//ִ���״ο�����־λ�жϣ�ȷ���Ƿ����״ο���
	
	log_info("[Dev_parameter_init]first_run_flag:0x%x,0x%x\r\n",temp[0],temp[1]);
	
	if( (temp[0] != 0x12) && (temp[1] != 0x34) )
	{
		//���״ο���
		
		AT45dbxx_EraseChip();	//flash����
		log_info("[Dev_parameter_init]CPU First run\r\n");
		Deviceinfo.Sample_count=0;
		Deviceinfo.id=DefaultDeviceADDR;
		Deviceinfo.dev_ty=DEVICETYPE;
		Deviceinfo.sv=SOFTWAREVERSION;
		Deviceinfo.work_time=0;
		Deviceinfo.temp_c=get_adc(1);//��ȡ�¶����ݣ��ѷŴ�100��
		Deviceinfo.bat_v=get_adc(0);//
		Deviceinfo.sample_interval=Default_SAMPLE_INTERVAL;
		Deviceinfo.rx_window=Default_RX_WINDOW;
		
		EEWrite(16,(void *)&Deviceinfo,sizeof(Deviceinfo));	
		
		//д�״ο�����־λ
		temp[0] = 0x12;
		temp[1] = 0x34;
		EEWrite(0x00,temp,2);
	}
	
	EERead(16,(void *)&Deviceinfo,sizeof(Deviceinfo));
	log_info("[Dev_parameter_init]id:0x%x,sv:0x%x,Sample_count:%d\r\n",Deviceinfo.id,Deviceinfo.sv,Deviceinfo.Sample_count);
	
#ifdef debug_log
	AT45_Log(1);//��ӡ��һҳ
#endif
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t sysclk=0;
uint32_t syscount=0;
volatile uint32_t worktime_buf=0;
volatile uint32_t samp_count=0;
volatile uint32_t re_window=0;
const char *g_Ashining = "HCF570_embedded_sofware_smartbow_hardware_zhaihaiqing_2020-02-05";
int main(void)
{
	uint32_t timesp=0;
	uint8_t test_buff[64]={0};
	uint16_t i = 0,j=0,k=0;
	HAL_Init();		//HAL���ʼ��
	Dev_ReInit();	//���Ӳ����ʼ��
#ifdef debug_log
	HAL_Get_CPU_RCC_Clock();	//��ӡʱ����Ϣ
#endif
	HAL_Delay(10);
	Dev_parameter_init();	//����豸������ʼ��
	
	
	//�豸��ʼ����
	log_info("[main]Hardware init OK!Cpu Run...\r\n");
	wakeup_flag=1;
	while (1)
	{
//		/*	alarm����������1h/�Σ�ִ�в������񣬽������ݴ洢�������豸״̬��Ϣ	*/
//		if(alarm_flag)	
//		{
//			log_info("[main]HAL_RTC_AlarmAEventCallback\r\n");
//			//Set_AlarmA(Deviceinfo.sample_interval*5);//����RTC����
//			alarm_flag=0;
//		}
		
		/*	wakeup����������10s/�Σ�����100ms���ȴ�ָ��������ݻ�ȡָ��豸��Ϣָ�RTC��ʱ	*/
		if(wakeup_flag)
		{
			log_info("[main]HAL_RTCEx_WakeUpTimerEventCallback\r\n");
#ifdef debug_log
			RTC_CalendarShow(&timesp);
#endif
			
			worktime_buf++;	//ÿ10s+1
			if(worktime_buf>=360)
			{
				worktime_buf=0;
				Deviceinfo.work_time++;
				EEWrite(28,(void *)&Deviceinfo.work_time,2);
			}
			
			samp_count++;
			log_info("samp_count:%d,Deviceinfo.sample_interval:%d\r\n",samp_count,Deviceinfo.sample_interval);
			if(samp_count>=Deviceinfo.sample_interval)//��Сʱ�䵥λ5s
			{
				samp_count=0;
				dev_data_sample_and_storage();//�ɼ����ݣ�ͬʱ�����洢
			}
			
			
			
			
			SI_POWER_ON();
			HAL_Delay(1);
			SI446x_Init();
			
//			for(k=0;k<64;k++)test_buff[k]=k;
//			for(k=0;k<64;k++){SI446x_TX_RX_Data(0,test_buff,64);HAL_Delay(2000);}	//���Ͳ�������
//			
			re_window=Deviceinfo.rx_window;
			log_info("[main]SI446x_Rxdata...\r\n");
			while(re_window)//��ʼ��ʱ100ms
			{
				SI446x_TX_RX_Data(1,wl_rx_buff,0);	//������������
				if(wl_rx_Flag)	//������������ִ��ָ�����
				{
					Instruction_Process();	
					
				}
			}
			SI_POWER_OFF();//���߽���͹���ģʽ
			HAL_Delay(1);
			wakeup_flag=0;
		}
		
		
		if(wakeup_flag==0)
		{
			enter_stopmode();
		}
				
	}
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
