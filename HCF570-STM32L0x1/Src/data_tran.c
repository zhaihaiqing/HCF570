/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*
���߲��ֲ���AS10-SMDģ�飬�ڲ�����SI4463оƬ����
*	֧��425-525MHz,Ƶ�ʿɵ�
*	����ͽ��հ��������64�ֽ�
*	�ɱ�̵Ŀ������ʣ�0.123kbps-1Mbps
*	���ֹ���ģʽ�����ٻ���ģʽ���͹���ģʽ������ģʽ������ģʽ
*	
*/

uint8_t wl_rx_buff[64]={0};		//������ջ���
uint8_t wl_tx_buff[64]={0};		//���巢�ͻ���


void Instruction_Process(void)
{
	if(wl_rx_buff[6] == Deviceinfo.id)//�Ǳ���ָ��
	{
		switch(wl_rx_buff[5])
		{
			case time_service:			//�����ּĴ���
						
						break;
			case set_sample_interval:			//������Ĵ���
						
						break;
			case inquire_info:		//д�������ּĴ���
						
						break;
			case data_request:
						
						break;
			default:
						break;
		}
	}
}









