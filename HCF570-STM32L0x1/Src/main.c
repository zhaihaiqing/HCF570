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


/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : HAL_Get_CPU_RCC_Clock
* Description    : 通过串口打印时钟信息
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
* Description    : 设备外设初始化
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
void Dev_ReInit(void)
{
	SystemClock_Config_MSI_1MHz();		/* Configure the system clock */
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_ADC_Init();
#ifdef debug_log
	MX_LPUART1_UART_Init();
#endif
	MX_RTC_Init();
	Set_Alarm();
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t sysclk=0;
uint32_t syscount=0;
int main(void)
{
	HAL_Init();
	Dev_ReInit();
	
	
	log_info("Hardware init OK!\r\n");
	log_info("Flashstorage Size:%d\r\n",sizeof(Datastorage));
	HAL_Get_CPU_RCC_Clock();
	
	AT45dbxx_Init();
	
//	Deviceinfo.id=0x01;
//	Deviceinfo.sv=0x32;
//	Deviceinfo.dataamount=3;
//	Deviceinfo.start_work_timestamp=0x123456;
//	
//	EEWrite(0,(void *)&Deviceinfo,12);
	
	HAL_Delay(2000);
	
	Deviceinfo.id=0x00;
	Deviceinfo.sv=0x0;
	Deviceinfo.dataamount=0;
	Deviceinfo.start_work_timestamp=0x0;
	
	EERead(0,(void *)&Deviceinfo,12);
	log_info("id:0x%x,sv:0x%x,dataamount:0x%x,start_work_timestamp:0x%x\r\n",Deviceinfo.id,Deviceinfo.sv,Deviceinfo.dataamount,Deviceinfo.start_work_timestamp);
	
	//AT45_test();
	
	
	while (1)
	{
		
		RTC_CalendarShow();
		
		//syscount=HAL_GetTick();
		get_adc();
		//syscount=HAL_GetTick()-syscount;
		//log_info("syscount:%d\r\n",syscount);
	  
		HAL_Delay(2000);
		//enter_stopmode();
		//HAL_Get_CPU_RCC_Clock();
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
