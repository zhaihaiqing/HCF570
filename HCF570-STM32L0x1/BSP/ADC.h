#ifndef __ADC_H
#define __ADC_H

extern ADC_HandleTypeDef hadc;

void MX_ADC_Init(void);


uint32_t get_adc(void);

#endif

