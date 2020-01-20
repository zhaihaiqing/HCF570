/* Includes ------------------------------------------------------------------*/
#include "main.h"


ADC_HandleTypeDef hadc;
ADC_ChannelConfTypeDef	sConfig;

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_ADCEx_Calibration_Start(&hadc,ADC_SINGLE_ENDED);
  
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
//  /** Configure for the selected ADC regular channel to be converted. 
//  */
//  sConfig.Channel = ADC_CHANNEL_2;
//  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
}

/*******************************************************************************
* Function Name  : get_adc
* Description    : 获取ADC对应通道的电压值
* Input          : N
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t get_adc(void)
{
	uint32_t adc_conv_var=0;
	float vdd_value=0,temp_value=0;
	
	HAL_ADCEx_EnableVREFINT();
	HAL_ADC_Stop(&hadc);
	hadc.Instance->CHSELR=0;
	sConfig.Channel=ADC_CHANNEL_17;
	HAL_ADC_ConfigChannel(&hadc,&sConfig);
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,10);
	
	if((HAL_ADC_GetState(&hadc)&HAL_ADC_STATE_REG_EOC)==HAL_ADC_STATE_REG_EOC)
	{
		adc_conv_var=HAL_ADC_GetValue(&hadc);
	}
	vdd_value=1.224*4095/adc_conv_var;				//计算出内部电源电压
	
	HAL_ADCEx_DisableVREFINT();
	HAL_ADC_Stop(&hadc);
	
	hadc.Instance->CHSELR=0;
	sConfig.Channel=ADC_CHANNEL_1;
	HAL_ADC_ConfigChannel(&hadc,&sConfig);
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,10);
	if((HAL_ADC_GetState(&hadc)&HAL_ADC_STATE_REG_EOC)==HAL_ADC_STATE_REG_EOC)
	{
		adc_conv_var=HAL_ADC_GetValue(&hadc);
	}
	
	temp_value=adc_conv_var*vdd_value/4095;
	
	HAL_ADC_Stop(&hadc);
	
	log_info("vdd_value:%f\r\n",vdd_value);
		
	return adc_conv_var;
}






__IO uint8_t   ubADC_overrun_status = RESET;

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* In this ADC LowPower example, ADC overrun is not considered as an error, */
  /* but as a way to watch the ADC low power modes effectiveness.             */
  /* Differentiation of ADC error overrun versus other potential errors:      */
  if (HAL_ADC_GetError(hadc) == HAL_ADC_ERROR_OVR)
  {
    /* Update variable to report ADC overrun event to main program */
    ubADC_overrun_status = SET;
  }
  else
  {
    /* In case of ADC error, call main error handler */
    Error_Handler();
  }
}




