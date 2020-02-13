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



extern uint8_t g_SI4463ItStatus[ 9 ];
extern uint8_t wl_tx_buff[ 64 ]; 
extern uint8_t wl_rx_buff[ 64 ]; 
extern uint8_t wl_rx_Flag;
extern uint8_t wl_rx_Len;

uint8_t SI446x_TX_RX_Data(uint8_t tx_rx,uint8_t *pbuff,uint8_t len);
uint8_t Instruction_Process(void);

uint8_t dev_TimeService(uint8_t *p_buf,uint8_t len);
uint8_t dev_Set_SampleInterval(uint8_t *p_buf,uint8_t len);


#endif

