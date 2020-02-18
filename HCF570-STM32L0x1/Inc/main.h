/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "Clock_Config.h"
#include "ADC.h"
#include "RTC.h"
#include "SPI.h"
#include "GPIO.h"
#include "uart.h"
#include "at45db.h"
#include "IEEPROM.h"
#include "lowpower.h"
#include "storage.h"
#include "data_tran.h"
#include "SI4463.h"
#include "SI4463_Config_30M.h"
#include "check.h"


#define LED1_ON()	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0)
#define LED1_OFF()	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1)
#define LED1_OR()	HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_4)

#define DefaultDeviceADDR	0x02	   	//Ĭ��������ַ
#define DEVICETYPE			0x023a	   //HCF570�豸����,��ʮ����710ת��Ϊʮ������
#define SOFTWAREVERSION		0x0400     //����汾�Ŷ�����򣺰汾�Ź���λ��ʾ������1.2.3����ʾ����Ϊ���߰�λ��ʾ1���Ͱ�λ��ʾ23,���еͰ�λ����ʾ99
#define Default_SAMPLE_INTERVAL		6			//Ĭ�����ݲɼ��������λ5s��720Ϊ1Сʱ
#define Default_RX_WINDOW	2000			//����ʱ�䴰�ڣ�100ms

extern IWDG_HandleTypeDef hiwdg;
extern uint8_t alarm_flag;
extern uint8_t wakeup_flag;
extern volatile uint32_t re_window;//����ʱ�䴰�ڼ�ʱ

void HAL_Get_CPU_RCC_Clock(void);
void Dev_ReInit(void);

void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
