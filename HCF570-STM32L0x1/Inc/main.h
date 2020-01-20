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


#define LED1_ON()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0)
#define LED1_OFF()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,1)
#define LED1_OR()	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_3)


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
