/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*
无线部分采用AS10-SMD模块，内部采用SI4463芯片方案
*	支持425-525MHz,频率可调
*	发射和接收包长度最大64字节
*	可编程的空中速率，0.123kbps-1Mbps
*	四种工作模式：快速唤醒模式，低功耗模式，发送模式，接收模式
*	
*/

uint8_t wl_rx_buff[64]={0};		//定义接收缓存
uint8_t wl_tx_buff[64]={0};		//定义发送缓存


void Instruction_Process(void)
{
	if(wl_rx_buff[6] == Deviceinfo.id)//是本机指令
	{
		switch(wl_rx_buff[5])
		{
			case time_service:			//读保持寄存器
						
						break;
			case set_sample_interval:			//读输入寄存器
						
						break;
			case inquire_info:		//写单个保持寄存器
						
						break;
			case data_request:
						
						break;
			default:
						break;
		}
	}
}









