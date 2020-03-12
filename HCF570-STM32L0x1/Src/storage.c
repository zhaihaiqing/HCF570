/* Includes ------------------------------------------------------------------*/
#include "main.h"

Datastorage_type 	Datastorage;
Deviceinfo_type     Deviceinfo;


/*	存储方案：
**	AT45DB041D/E默认每页264byte
**	每组数据8byte,
**	每页最后8byte不用于存储数据，每页能存储32条数据
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
	temp16=get_adc(1);//获取温度数据
	
	log_info("dev_data_sample_and_storage-timestamp:%d\r\n",timestamp);
	Datastorage.iddet1=0x23;
	Datastorage.temp=temp16;		//temp16=真实温度*100，存入flash
	Datastorage.timestamp=timestamp;
	Datastorage.sumcheck=(unsigned char)(Check_cumulative_sum((void *)&Datastorage,sizeof(Datastorage)-1) & 0xff);
	
	Deviceinfo.Sample_count++;
	log_info("Write samp_count to eeprom!\r\n");
	EEWrite(16,(void *)&Deviceinfo.Sample_count,4);//记录存储数据的数量
	
	
	/*
	page_num:1-n
	in_page_num:0-31
		组1-32，存储在page1
		组33-64，存储在page2
		组65-96，存储在page3
	*/
	
	page_num=(Deviceinfo.Sample_count-1)/32+1;//计算本次应该存储的页号
	in_page_num=(Deviceinfo.Sample_count-1)%32;//计算本次应该存储在页内的位置(0-31,共32组数)
	
	log_info("[dev_data_sample_and_storage]Sample_count:%d,page_num:%d,in_page_num:%d\r\n",Deviceinfo.Sample_count,page_num,in_page_num);
	
	AT45dbxx_ReadPage(Flash_buff,264,page_num);//将应该存储的页内容全部读出
	
	Flash_buff[in_page_num*8]=Datastorage.iddet1;		//将数据添加在对应的位置										
	Flash_buff[in_page_num*8+1]=(Datastorage.temp&0xff00)>>8;
	Flash_buff[in_page_num*8+2]=Datastorage.temp&0xff;
	Flash_buff[in_page_num*8+3]=(Datastorage.timestamp&0xff000000)>>24;
	Flash_buff[in_page_num*8+4]=(Datastorage.timestamp&0xff0000)>>16;									
	Flash_buff[in_page_num*8+5]=(Datastorage.timestamp&0xff00)>>8;
	Flash_buff[in_page_num*8+6]=Datastorage.timestamp&0xff;
	Flash_buff[in_page_num*8+7]=Datastorage.sumcheck;
	
	if(page_num<2000)
		AT45dbxx_WritePage(Flash_buff,264,page_num);//将数据进行存储
	
//#ifdef debug_log
//	AT45_Log(page_num);//打印第一页
//#endif
}

