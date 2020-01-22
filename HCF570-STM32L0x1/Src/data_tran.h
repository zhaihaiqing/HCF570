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




extern uint8_t wl_rx_buff[64];		//定义接收缓存
extern uint8_t wl_tx_buff[64];		//定义发送缓存


void Instruction_Process(void);


#endif

