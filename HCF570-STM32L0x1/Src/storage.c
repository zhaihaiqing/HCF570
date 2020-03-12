/* Includes ------------------------------------------------------------------*/
#include "main.h"

Datastorage_type 	Datastorage;
Deviceinfo_type     Deviceinfo;


/*	�洢������
**	AT45DB041D/EĬ��ÿҳ264byte
**	ÿ������8byte,
**	ÿҳ���8byte�����ڴ洢���ݣ�ÿҳ�ܴ洢32������
*/

void dev_data_sample_and_storage(void)
{
	unsigned char Flash_buff[264]={0};
	
	short temp16=0;
	short temp8=0;
	uint32_t timestamp=0;
	uint16_t page_num=0;
	uint16_t in_page_num=0;
	
	
	RTC_CalendarShow(&timestamp);
	temp16=get_adc(1);//��ȡ�¶�����
	
	log_info("dev_data_sample_and_storage-timestamp:%d\r\n",timestamp);
	Datastorage.iddet1=0x23;
	Datastorage.temp=temp16;		//temp16=��ʵ�¶�*100������flash
	Datastorage.timestamp=timestamp;
	Datastorage.sumcheck=(unsigned char)(Check_cumulative_sum((void *)&Datastorage,sizeof(Datastorage)-1) & 0xff);
	
	Deviceinfo.Sample_count++;
	log_info("Write samp_count to eeprom!\r\n");
	EEWrite(16,(void *)&Deviceinfo.Sample_count,4);//��¼�洢���ݵ�����
	
	
	/*
	page_num:1-n
	in_page_num:0-31
		��1-32���洢��page1
		��33-64���洢��page2
		��65-96���洢��page3
	*/
	
	page_num=(Deviceinfo.Sample_count-1)/32+1;//���㱾��Ӧ�ô洢��ҳ��
	in_page_num=(Deviceinfo.Sample_count-1)%32;//���㱾��Ӧ�ô洢��ҳ�ڵ�λ��(0-31,��32����)
	
	log_info("[dev_data_sample_and_storage]Sample_count:%d,page_num:%d,in_page_num:%d\r\n",Deviceinfo.Sample_count,page_num,in_page_num);
	
	AT45dbxx_ReadPage(Flash_buff,264,page_num);//��Ӧ�ô洢��ҳ����ȫ������
	
	Flash_buff[in_page_num*8]=Datastorage.iddet1;		//����������ڶ�Ӧ��λ��										
	Flash_buff[in_page_num*8+1]=(Datastorage.temp&0xff00)>>8;
	Flash_buff[in_page_num*8+2]=Datastorage.temp&0xff;
	Flash_buff[in_page_num*8+3]=(Datastorage.timestamp&0xff000000)>>24;
	Flash_buff[in_page_num*8+4]=(Datastorage.timestamp&0xff0000)>>16;									
	Flash_buff[in_page_num*8+5]=(Datastorage.timestamp&0xff00)>>8;
	Flash_buff[in_page_num*8+6]=Datastorage.timestamp&0xff;
	Flash_buff[in_page_num*8+7]=Datastorage.sumcheck;
	
	if(page_num<2000)
		AT45dbxx_WritePage(Flash_buff,264,page_num);//�����ݽ��д洢
	
//#ifdef debug_log
//	AT45_Log(page_num);//��ӡ��һҳ
//#endif
}

