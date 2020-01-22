#ifndef __DATA_TRAN_H
#define __DATA_TRAN_H
#include "stm32l0xx.h"

enum
{
	time_service=1,
	set_sample_interval,
	inquire_info,
	data_request,
	
};




extern uint8_t wl_rx_buff[64];		//������ջ���
extern uint8_t wl_tx_buff[64];		//���巢�ͻ���


void Instruction_Process(void);


#endif

