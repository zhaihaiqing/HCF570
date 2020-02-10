#ifndef __ADC_H
#define __ADC_H


#define OP_POWER_ON()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,1)
#define OP_POWER_OFF()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,0)

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC1_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC1_RELEASE_RESET()

extern ADC_HandleTypeDef hadc;

void MX_ADC_Init(void);
double get_vdd_v(void);
double get_temp_t(double vref);
short get_adc(uint8_t val_sel);

#endif

