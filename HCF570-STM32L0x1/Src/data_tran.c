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

//#define		DEV_ADD_2BYTES
#define		DEV_ADD_4BYTES

uint8_t g_TxMode = 0, g_UartRxFlag = 0;

uint8_t g_SI4463ItStatus[ 9 ] = { 0 };
uint8_t wl_tx_buff[ 64 ] = { 0 }; 
uint8_t wl_rx_buff[ 64 ] = { 0 }; 
uint8_t wl_rx_Flag=0;
uint8_t wl_rx_Len=0;

void clear_wl_rx_buff(void)
{
	wl_rx_Flag=0;
	wl_rx_Len=0;
	memset(wl_rx_buff,0,64);
}


/*******************************************************************************
* Function Name  : SI446x_TX_RX_Data
* Description    : SI446x
* Input          : tx_rx：0-tx,1-rx，pbuff：发送或接收的数据指针，len：发送数据的长度（接收时不用）
* Output         : None
* Return         : 成功返回0，失败返回1
*******************************************************************************/
uint8_t SI446x_TX_RX_Data(uint8_t tx_rx,uint8_t *pbuff,uint8_t len)
{
	uint16_t i=0,j=0;
	if(tx_rx==0)//发送模式
	{
		if((len<1) || (len>64))return 1;						//发送字节长度必须大于0，且不大于64
		SI446x_Send_Packet( (uint8_t *)pbuff, len, 0, 0 );		//发送数据，发送完成后切换为接收模式
		
		SI446x_Change_Status(SI4463_Mode_Tune_State_For_RX);	//切换到RX状态
		while( SI4463_Mode_Tune_State_For_RX != SI446x_Get_Device_Status( ));
		SI446x_Start_Rx(  0, 0, PACKET_LENGTH,0,0,3 );
	}
	
	else if(tx_rx==1)//接收模式，不关心len
	{
		SI446x_Interrupt_Status( g_SI4463ItStatus );		//读中断状态
		if( g_SI4463ItStatus[ 3 ] & 0x10)	
		{
			i = SI446x_Read_Packet( pbuff );				//读FIFO数据
			if( i != 0 )
			{
				wl_rx_Flag=1;//接收完成标志位置位
				wl_rx_Len=i;
#ifdef debug_log				
					HAL_UART_Transmit(&hlpuart1,(uint8_t *)&pbuff[j], wl_rx_Len, 0xFFFF);
#endif
				//log_info("[SI446x_TX_RX_Data]SI4463_RX_Complete,len=%d\r\n",i);
			}
			SI446x_Change_Status( SI4463_Mode_Tune_State_For_RX );
			while( SI4463_Mode_Tune_State_For_RX != SI446x_Get_Device_Status( ));
			SI446x_Start_Rx(  0, 0, PACKET_LENGTH,0,0,3 );
		}
//		else		
//		{
//			if( 3000 == i++ )
//			{
//				i = 0;
//				SI446x_Init( );
//			}
//			HAL_Delay( 1 );
//		}
	}
	
	return 0;
}

/*******************************************************************************
* Function Name  : dev_TimeService
* Description    : 完成设备授时
* Input          : p_buf：接收数据指针，len：接收数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void dev_TimeService(uint8_t *p_buf,uint8_t len)
{
	struct tm dev_time;		//定义时间结构体
	struct tm *p_time=&dev_time;//定义指向时间结构体的指针
	time_t timestamp=0;
	uint16_t checksum=0;
	
	uint8_t SI_ack[64]={0};
#ifdef DEV_ADD_2BYTES		
	timestamp=(p_buf[10]<<24) | (p_buf[11]<<16) | (p_buf[12]<<8) | p_buf[13];//获取时间戳
#endif
#ifdef DEV_ADD_4BYTES		
	timestamp=(p_buf[14]<<24) | (p_buf[15]<<16) | (p_buf[16]<<8) | p_buf[17];//获取时间戳
#endif
	log_info("[dev_TimeService]timestamp:0x%x\r\n",timestamp);
	RTC_Timestamp_To_DateTime(timestamp,p_time);//将时间戳转换为时间
	log_info("[dev_TimeService]tm_year:%d-",dev_time.tm_year);
	log_info("%d-",dev_time.tm_mon);
	log_info("%d ",dev_time.tm_mday);
	log_info("%d:",dev_time.tm_hour);
	log_info("%d:",dev_time.tm_min);
	log_info("%d\r\n",dev_time.tm_sec);
	RTC_SetDataTime(dev_time.tm_year,dev_time.tm_mon,dev_time.tm_mday,dev_time.tm_hour,dev_time.tm_min,dev_time.tm_sec);//设置时间			
	
#ifdef DEV_ADD_2BYTES
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x01;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	checksum=Check_cumulative_sum(&SI_ack[5],5);
	SI_ack[10]=(checksum&0xff00)>>8;
	SI_ack[11]=checksum&0xff;

	//ACK
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,12);
	HAL_Delay(15);
#endif

#ifdef DEV_ADD_4BYTES
	
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x01;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	checksum=Check_cumulative_sum(&SI_ack[5],9);
	SI_ack[14]=(checksum&0xff00)>>8;
	SI_ack[15]=checksum&0xff;

	//ACK
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	HAL_Delay(20);
	
#endif
	
	
}

/*******************************************************************************
* Function Name  : dev_Set_SampleInterval
* Description    : 设置采样时间间隔和通信时间窗口
* Input          : p_buf：接收数据指针，len：接收数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void dev_Set_SampleInterval(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint16_t checksum=0;
	
	uint32_t sample_interval=0;
	uint16_t receive_window=0;
	
#ifdef DEV_ADD_2BYTES		
	sample_interval=(p_buf[10]<<24) | (p_buf[11]<<16) | (p_buf[12]<<8) | p_buf[13];
	receive_window=p_buf[14]<<8) | p_buf[15];
#endif
#ifdef DEV_ADD_4BYTES		
	sample_interval=((p_buf[14]<<24) | (p_buf[15]<<16) | (p_buf[16]<<8) | p_buf[17])*5;
	receive_window=(p_buf[18]<<8) | p_buf[19];
#endif
	
	EEWrite(34,(void *)&sample_interval,4);	//存储数据
	EEWrite(38,(void *)&receive_window,2);		//存储数据
	Deviceinfo.sample_interval=sample_interval;			//更新全局变量
	Deviceinfo.rx_window=receive_window;				//更新全局变量
	Set_AlarmA(Deviceinfo.sample_interval);				//更新RTC闹钟
	
#ifdef DEV_ADD_2BYTES
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x00;
	
	SI_ack[5]=0x01;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	checksum=Check_cumulative_sum(&SI_ack[5],5);
	SI_ack[10]=(checksum&0xff00)>>8;
	SI_ack[11]=checksum&0xff;

	//ACK
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,12);
	HAL_Delay(15);
#endif

#ifdef DEV_ADD_4BYTES
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x00;
	
	SI_ack[5]=0x01;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	checksum=Check_cumulative_sum(&SI_ack[5],9);
	SI_ack[14]=(checksum&0xff00)>>8;
	SI_ack[15]=checksum&0xff;

	//ACK
	log_info("time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	HAL_Delay(20);
#endif
}

/*******************************************************************************
* Function Name  : dev_inquire_info
* Description    : 请求设备信息
* Input          : p_buf：接收数据指针，len：接收数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void dev_inquire_info(uint8_t *p_buf,uint8_t len)
{
	
	uint32_t Sample_interval=0;	//采集时间间隔，不配置时，默认1h
	uint16_t Receive_Window=0;	//接收时间窗口，不配置时，默认100ms
	uint8_t SI_ack[64]={0};
	uint32_t timestamp=0;
	uint16_t battery_val=0;
	uint16_t total_worktime=0;
	uint16_t SoftwareVersion=0;
	uint16_t checksum=0;
	
	
	RTC_CalendarShow(&timestamp);//获取时间戳
	Receive_Window=Deviceinfo.rx_window;
	battery_val=get_adc(0);
	total_worktime=Deviceinfo.work_time;
	SoftwareVersion=Deviceinfo.sv;

	log_info("[dev_inquire_info]timestamp:%d,Receive_Window:%dms,battery_val:%.2f,total_worktime:%dh,SoftwareVersion:0x%x\r\n",\
				timestamp,Receive_Window,battery_val/5000.0,total_worktime,SoftwareVersion);
	log_info("[dev_inquire_info]Storage_count:%d\r\n",Deviceinfo.Sample_count);

#ifdef DEV_ADD_2BYTES
	//发送回执
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x03;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	
	checksum=Check_cumulative_sum(&SI_ack[5],5);
	SI_ack[10]=(checksum&0xff00)>>8;
	SI_ack[11]=checksum&0xff;
	//ACK
	log_info("[dev_inquire_info]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,12);
	HAL_Delay(15);
	
	//上传数据
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0x80;
	SI_ack[3]=0;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x03;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	
	SI_ack[10]=(timestamp&0xff000000)>>24;
	SI_ack[11]=(timestamp&0xff0000)>>16;
	SI_ack[12]=(timestamp&0xff00)>>8;
	SI_ack[13]=timestamp&0xff;
	
	SI_ack[14]=(Receive_Window&0xff00)>>8;
	SI_ack[15]=Receive_Window&0xff;
	
	SI_ack[16]=(battery_val&0xff00)>>8;
	SI_ack[17]=battery_val&0xff;
	
	SI_ack[18]=(total_worktime&0xff00)>>8;
	SI_ack[19]=total_worktime&0xff;
	
	SI_ack[20]=(SoftwareVersion&0xff00)>>8;
	SI_ack[21]=SoftwareVersion&0xff;
	
	checksum=Check_cumulative_sum(&SI_ack[5],17);
	SI_ack[22]=(checksum&0xff00)>>8;
	SI_ack[23]=checksum&0xff;
	
	//ACK
	log_info("[dev_inquire_info]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,24);
	HAL_Delay(30);
#endif

#ifdef DEV_ADD_4BYTES
	//发送回执
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x03;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	checksum=Check_cumulative_sum(&SI_ack[5],9);
	SI_ack[14]=(checksum&0xff00)>>8;
	SI_ack[15]=checksum&0xff;
	//ACK
	log_info("[dev_inquire_info]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	HAL_Delay(20);
	
	//上传数据
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0x80;
	SI_ack[3]=0;
	SI_ack[4]=0x05;
	
	SI_ack[5]=0x03;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	SI_ack[14]=(timestamp&0xff000000)>>24;
	SI_ack[15]=(timestamp&0xff0000)>>16;
	SI_ack[16]=(timestamp&0xff00)>>8;
	SI_ack[17]=timestamp&0xff;
	
	SI_ack[18]=(Receive_Window&0xff00)>>8;
	SI_ack[19]=Receive_Window&0xff;
	
	SI_ack[20]=(battery_val&0xff00)>>8;
	SI_ack[21]=battery_val&0xff;
	
	SI_ack[22]=(total_worktime&0xff00)>>8;
	SI_ack[23]=total_worktime&0xff;
	
	SI_ack[24]=(SoftwareVersion&0xff00)>>8;
	SI_ack[25]=SoftwareVersion&0xff;
	
	checksum=Check_cumulative_sum(&SI_ack[5],21);
	SI_ack[26]=(checksum&0xff00)>>8;
	SI_ack[27]=checksum&0xff;
	
	//ACK
	log_info("[dev_inquire_info]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,28);
	HAL_Delay(35);
#endif
	
}


void dev_Send_EOT(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint16_t checksum=0;
	
#ifdef DEV_ADD_2BYTES
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x00;
	
	SI_ack[5]=0x02;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	checksum=Check_cumulative_sum(&SI_ack[5],5);
	SI_ack[10]=(checksum&0xff00)>>8;
	SI_ack[11]=checksum&0xff;

	//ACK
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,12);
	HAL_Delay(15);
#endif

#ifdef DEV_ADD_4BYTES
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x00;
	
	SI_ack[5]=0x02;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	checksum=Check_cumulative_sum(&SI_ack[5],9);
	SI_ack[14]=(checksum&0xff00)>>8;
	SI_ack[15]=checksum&0xff;

	//ACK
	log_info("time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	HAL_Delay(20);
#endif
}

uint8_t dev_Extract_Ack(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint16_t checksum=0;
	uint32_t dID=0;
	uint8_t ack_nack=0;
	
#ifdef DEV_ADD_2BYTES
		dID=(p_buf[8]<<8) | p_buf[9];//获取目的地址
#endif
	
#ifdef DEV_ADD_4BYTES
		dID=(p_buf[10]<<24) | (p_buf[11]<<16) | (p_buf[12]<<8) | p_buf[13];//获取目的地址
#endif

	dID=(p_buf[10]<<24) | (p_buf[11]<<16) | (p_buf[12]<<8) | p_buf[13];//获取目的地址
	checksum=Check_cumulative_sum(&p_buf[5],len-7);//判断校验和
	if( ((p_buf[0]!=0x55) || (p_buf[1]!=0xaa))	\
			&&(checksum == ( p_buf[len-2]<<8) || p_buf[len-1]) \
			&& (dID==Deviceinfo.id) )	//帧头判断
										//校验和验证
										//地址匹配
	{
			ack_nack=p_buf[5];
	}
	else
		ack_nack=0xff;
	
	return ack_nack;
}

/*******************************************************************************
* Function Name  : dev_data_request
* Description    : 请求数据
* Input          : p_buf：接收数据指针，len：接收数据长度
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t dev_data_request(uint8_t *p_buf,uint8_t len)
{
	uint8_t SI_ack[64]={0};
	uint8_t flash_buf[264]={0};
	uint32_t data_count=0;
	uint16_t page_count=0;
	uint16_t in_page_num=0;
	uint16_t send_num=0;
	uint16_t i=0,j=0,k=0;
	uint16_t checksum=0;
	uint8_t  send_frame_num=0;
	uint16_t timeout=0;
	uint32_t dID=0;
	uint8_t ack_nack=0;
	
	/* 发送回执信息  */
#ifdef DEV_ADD_2BYTES	
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x09;
	
	SI_ack[5]=0x04;
	SI_ack[6]=p_buf[8];
	SI_ack[7]=p_buf[9];
	SI_ack[8]=p_buf[6];
	SI_ack[9]=p_buf[7];
	checksum=Check_cumulative_sum(&SI_ack[5],5);
	SI_ack[10]=(checksum&0xff00)>>8;
	SI_ack[11]=checksum&0xff;
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,12);
	HAL_Delay(20);
#endif
	
#ifdef DEV_ADD_4BYTES
	
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x09;
	
	SI_ack[5]=0x04;
	SI_ack[6]=p_buf[10];
	SI_ack[7]=p_buf[11];
	SI_ack[8]=p_buf[12];
	SI_ack[9]=p_buf[13];
	
	SI_ack[10]=p_buf[6];
	SI_ack[11]=p_buf[7];
	SI_ack[12]=p_buf[8];
	SI_ack[13]=p_buf[9];
	
	checksum=Check_cumulative_sum(&SI_ack[5],9);
	SI_ack[14]=(checksum&0xff00)>>8;
	SI_ack[15]=checksum&0xff;

	//ACK
	log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	HAL_Delay(30);
	
#endif
	
	/*	
		数据传输
		一帧数据包含8个数据点
		1B包号
		每隔数据点包含5个字节，1B温度+4B时间戳
		2B校验和
		
	*/
	data_count=Deviceinfo.Sample_count;				//获取当前数据条数
	page_count=(data_count-1)/32+1;					//计算最后一条数据所在的页号
	in_page_num=(data_count-1)%32;					//计算最后一条数据所在的组号
	send_num=(data_count-1)/8+1;					//计算所需要发送的数据包数量
	
	for(i=1;i<=page_count;i++)
	{
		AT45dbxx_ReadPage(flash_buf,264,i);			//读对应的页，每页32组数据，分为4包，每包8组数
		if(i!=page_count)							
		{
			for(j=0;j<4;j++)	//分为4包发送
			{
				SI_ack[0]=0x55;
				SI_ack[1]=0xaa;
				SI_ack[2]=0x80;
				SI_ack[3]=p_buf[3]+1;
				SI_ack[4]=0x29;
	
				SI_ack[5]=0x04;
				SI_ack[6]=p_buf[10];
				SI_ack[7]=p_buf[11];
				SI_ack[8]=p_buf[12];
				SI_ack[9]=p_buf[13];
	
				SI_ack[10]=p_buf[6];
				SI_ack[11]=p_buf[7];
				SI_ack[12]=p_buf[8];
				SI_ack[13]=p_buf[9];
				SI_ack[14]=send_frame_num;
				
				for(k=0;k<=7;k++)				//每包包含8个数据点
				{
					SI_ack[14+k*5+1]=(((flash_buf[j*64+k*8+1])<<8|flash_buf[j*64+k*8+2])/100+40)/0.5;		//转换为单字节温度;
					SI_ack[14+k*5+2]=flash_buf[j*64+k*8+3];
					SI_ack[14+k*5+3]=flash_buf[j*64+k*8+4];
					SI_ack[14+k*5+4]=flash_buf[j*64+k*8+5];
					SI_ack[14+k*5+5]=flash_buf[j*64+k*8+6];
				}

				checksum=Check_cumulative_sum(&SI_ack[5],50);
				SI_ack[55]=(checksum&0xff00)>>8;
				SI_ack[56]=checksum&0xff;
	
		try_send_again1:	//如果发生错误，重传，最大次数5
				SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,57);
				
				HAL_Delay(60);
				
				
				//等待ACK,10s内无响应则退出
				timeout=5000;
				while(timeout--)//开始计时100ms
				{
					SI446x_TX_RX_Data(1,wl_rx_buff,0);	//接收无线数据
					if(wl_rx_Flag)	//如果接收完成则执行指令处理函数
					{	
						ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//解析ack类型
						if(ack_nack==0)//收到ack,继续发送
						{
							send_frame_num++;//发送成功，send_frame_num++
							clear_wl_rx_buff();
							break;
						}
						else if(ack_nack==1)//收到nack，重传
						{
							clear_wl_rx_buff();
							goto try_send_again1;
						}
						else if(ack_nack==6)//收到can，采集器主动取消数据传输
						{
					send_EOT1:
							dev_Send_EOT(wl_rx_buff,wl_rx_Len);//发送EOT
							clear_wl_rx_buff();
							while(1)
							{
								SI446x_TX_RX_Data(1,wl_rx_buff,0);	//接收无线数据
								if(wl_rx_Flag)
								{
									ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//解析ack类型
									clear_wl_rx_buff();
									if(ack_nack==1)goto send_EOT1;
									if(ack_nack==0)break;
								}
							}
							
							return 1;
						}	
						
					}
					HAL_Delay(2);
					if(timeout==0)return 1;//如果超时，退出发送
				}
			}
			
		}
		else					//最后一页
		{
			for(j=0;j<in_page_num/8+1;j++)	//分为in_page_num/8+1包发送，in_page_num(0-31)
			{
				SI_ack[0]=0x55;
				SI_ack[1]=0xaa;
				SI_ack[2]=0x80;
				SI_ack[3]=p_buf[3]+1;
				SI_ack[4]=0x29;
	
				SI_ack[5]=0x04;
				SI_ack[6]=p_buf[10];
				SI_ack[7]=p_buf[11];
				SI_ack[8]=p_buf[12];
				SI_ack[9]=p_buf[13];
	
				SI_ack[10]=p_buf[6];
				SI_ack[11]=p_buf[7];
				SI_ack[12]=p_buf[8];
				SI_ack[13]=p_buf[9];
				SI_ack[14]=send_frame_num;
				
				for(k=0;k<=7;k++)				//每包包含8个数据点
				{
					SI_ack[14+k*5+1]=(((flash_buf[j*64+k*8+1])<<8|flash_buf[j*64+k*8+2])/100+40)/0.5;		//转换为单字节温度;
					SI_ack[14+k*5+2]=flash_buf[j*64+k*8+3];
					SI_ack[14+k*5+3]=flash_buf[j*64+k*8+4];
					SI_ack[14+k*5+4]=flash_buf[j*64+k*8+5];
					SI_ack[14+k*5+5]=flash_buf[j*64+k*8+6];
				}

				checksum=Check_cumulative_sum(&SI_ack[5],50);
				SI_ack[55]=(checksum&0xff00)>>8;
				SI_ack[56]=checksum&0xff;
	
		try_send_again2:	//如果发生错误，重传，最大次数5
				SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,57);
				
				HAL_Delay(60);
				
				
				//等待ACK,10s内无响应则退出
				timeout=5000;
				while(timeout--)//开始计时100ms
				{
					SI446x_TX_RX_Data(1,wl_rx_buff,0);	//接收无线数据
					if(wl_rx_Flag)	//如果接收完成则执行指令处理函数
					{	
						ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//解析ack类型
						if(ack_nack==0)//收到ack,继续发送
						{
							send_frame_num++;//发送成功，send_frame_num++
							clear_wl_rx_buff();
							break;
						}
						else if(ack_nack==1)//收到nack，重传
						{
							clear_wl_rx_buff();
							goto try_send_again2;
						}
						else if(ack_nack==6)//收到can，采集器主动取消数据传输
						{
					send_EOT2:
							dev_Send_EOT(wl_rx_buff,wl_rx_Len);//发送EOT
							clear_wl_rx_buff();
							while(1)
							{
								SI446x_TX_RX_Data(1,wl_rx_buff,0);	//接收无线数据
								if(wl_rx_Flag)
								{
									ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//解析ack类型
									clear_wl_rx_buff();
									if(ack_nack==1)goto send_EOT2;
									if(ack_nack==0)break;
								}
							}
							
							return 1;
						}	
						
					}
					HAL_Delay(2);
					if(timeout==0)return 1;//如果超时，退出发送
				}
			}
		}
		
	}
	
	
	
	
	
	
	
	
	

}

uint8_t Instruction_Process(void)
{	
	uint32_t dID=0;
	uint16_t checksum=0;
	
	uint8_t data_buf[64]={0};
	uint8_t	rx_len=0;
	
	rx_len=wl_rx_Len;
	memcpy(data_buf,wl_rx_buff,64);
	
	wl_rx_Flag=0;
	wl_rx_Len=0;
	memset(wl_rx_buff,0,64);	//释放缓存
	
	if((data_buf[0]!=0x55) || (data_buf[1]!=0xaa))return 1;//判断帧头
	if(data_buf[2]!=0x20 )return 1;//如果不是下行指令则退出	
	checksum=Check_cumulative_sum(&data_buf[5],rx_len-7);//判断校验和
	if(checksum != ( data_buf[rx_len-2]<<8) || data_buf[rx_len-1]  ) return 1;//校验和错误
	
#ifdef DEV_ADD_2BYTES
	dID=(data_buf[8]<<8) | data_buf[9];//获取目的地址
#endif
#ifdef DEV_ADD_4BYTES
	//	sID=(data_buf[6]<<24) | (data_buf[7]<<16) | (data_buf[8]<<8) | data_buf[9];//获取目的地址
	dID=(data_buf[10]<<24) | (data_buf[11]<<16) | (data_buf[12]<<8) | data_buf[13];//获取目的地址
#endif
	
	if(dID == Deviceinfo.id)//是本机指令
	{
		switch(data_buf[5])
		{
			case time_service:			//授时
						dev_TimeService(data_buf,rx_len);
						break;
			case set_sample_interval:	//设置采样间隔
						dev_Set_SampleInterval(data_buf,rx_len);
						break;
			case inquire_info:			//请求设备信息
						dev_inquire_info(data_buf,rx_len);
						break;
			case data_request:			//请求数据
						dev_data_request(data_buf,rx_len);	//接收到数据请求指令
						break;
			default:
						break;
		}
	}
}









