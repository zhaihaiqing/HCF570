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
	log_info("HAL_RCC_SysClockFreq:%fMHz\r\n",1.0*HAL_RCC_GetSysClockFreq()/1000000);
	log_info("HAL_RCC_HCLKFreq:%fMHz\r\n",1.0*HAL_RCC_GetHCLKFreq()/1000000);
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

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t sysclk=0;
uint32_t syscount=0;
volatile uint32_t re_window=100;
int main(void)
{
	uint8_t i=0;
	HAL_Init();
	Dev_ReInit();
	
	log_info("Hardware init OK!\r\n");
	log_info("Flashstorage Size:%d\r\n",sizeof(Datastorage));
	HAL_Get_CPU_RCC_Clock();
	
	Deviceinfo.id=0x01;
	Deviceinfo.sv=0x32;
	Deviceinfo.dataamount=3;
	Deviceinfo.start_work_timestamp=0x123456;
	
	EEWrite(0,(void *)&Deviceinfo,12);
	
	HAL_Delay(2000);
	
	Deviceinfo.id=0x00;
	Deviceinfo.sv=0x0;
	Deviceinfo.dataamount=0;
	Deviceinfo.start_work_timestamp=0x0;
	
	EERead(0,(void *)&Deviceinfo,12);
	log_info("id:0x%x,sv:0x%x,dataamount:0x%x,start_work_timestamp:0x%x\r\n",Deviceinfo.id,Deviceinfo.sv,Deviceinfo.dataamount,Deviceinfo.start_work_timestamp);

	log_info("Start\r\n");
	
	while (1)
	{
		/*	alarm����������1h/�Σ�ִ�в������񣬽������ݴ洢�������豸״̬��Ϣ	*/
		if(alarm_flag)	
		{
			log_info("HAL_RTC_AlarmAEventCallback\r\n");
			RTC_CalendarShow();
			get_adc();
			//�����洢
			alarm_flag=0;
		}
		
		/*	wakeup����������10s/�Σ�����100ms���ȴ�ָ��������ݻ�ȡָ��豸��Ϣָ�RTC��ʱ	*/
		if(wakeup_flag)
		{
			log_info("HAL_RTCEx_WakeUpTimerEventCallback\r\n");
			RTC_CalendarShow();
			AT45_test();
			
			log_info("SI_POWER_ON\r\n");
			SI_POWER_ON();
			HAL_Delay(5);
			SI446x_Init();
			
			re_window=100;
			while(re_window)//��ʼ��ʱ100ms
			{
				//����ָ��
				//����ָ��
				Instruction_Process();
			}
			SI_POWER_OFF();//���߽���͹���ģʽ
			HAL_Delay(5);
			wakeup_flag=0;
		}
		
		if((alarm_flag==0)&&(wakeup_flag==0))
		{
			enter_stopmode();	//����͹���ģʽ
			//HAL_Get_CPU_RCC_Clock();
		}
		HAL_Delay(5);
		
		
		
		
		//syscount=HAL_GetTick();
		
		//syscount=HAL_GetTick()-syscount;
		//log_info("syscount:%d\r\n",syscount);
	  
		
		
		
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
