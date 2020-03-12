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
* Input          : tx_rx��0-tx,1-rx��pbuff�����ͻ���յ�����ָ�룬len���������ݵĳ��ȣ�����ʱ���ã�
* Output         : None
* Return         : �ɹ�����0��ʧ�ܷ���1
*******************************************************************************/
uint8_t SI446x_TX_RX_Data(uint8_t tx_rx,uint8_t *pbuff,uint8_t tx_len)
{
	uint16_t rx_len=0,j=0;
	if(tx_rx==0)//����ģʽ
	{
		if((tx_len<1) || (tx_len>64))return 1;						//�����ֽڳ��ȱ������0���Ҳ�����64
		SI446x_Send_Packet( (uint8_t *)pbuff, tx_len, 0, 0 );		//�������ݣ�������ɺ��л�Ϊ����ģʽ
		
		//HAL_Delay(tx_len*1.2);
		SI446x_Init();						//״̬�л����ȶ���ֱ�ӵ��ó�ʼ���������ʼ��Ϊ����ģʽ
		
//		SI446x_Change_Status(SI4463_Mode_Tune_State_For_RX);	//�л���RX״̬
//		while( SI4463_Mode_Tune_State_For_RX != SI446x_Get_Device_Status( ));
//		SI446x_Start_Rx(  0, 0, PACKET_LENGTH,0,0,3 );
	}
	
	else if(tx_rx==1)//����ģʽ��������len
	{
		SI446x_Interrupt_Status( g_SI4463ItStatus );		//���ж�״̬
		if( g_SI4463ItStatus[ 3 ] & 0x10)	
		{
			rx_len = SI446x_Read_Packet( pbuff );				//��FIFO����
			if( rx_len != 0 )
			{
				wl_rx_Flag=1;//������ɱ�־λ��λ
				wl_rx_Len=rx_len;
//#ifdef debug_log				
//					HAL_UART_Transmit(&hlpuart1,(uint8_t *)&pbuff[j], wl_rx_Len, 0xFFFF);
//#endif
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
* Description    : ����豸��ʱ
* Input          : p_buf����������ָ�룬len���������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t dev_TimeService(uint8_t *p_buf,uint8_t len)
{
	struct tm dev_time;		//����ʱ��ṹ��
	struct tm *p_time=&dev_time;//����ָ��ʱ��ṹ���ָ��
	time_t timestamp=0;
	uint16_t checksum=0;
	
	uint8_t SI_ack[64]={0};

	timestamp=(p_buf[14]<<24) | (p_buf[15]<<16) | (p_buf[16]<<8) | p_buf[17];//��ȡʱ���
	
	if((timestamp<946656000) || (timestamp>2524579199))return 1;
	
	//HAL_Delay(10);
	
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x09;
	
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
	//log_info("[dev_TimeService]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	//HAL_Delay(20);
	
	
	log_info("[dev_TimeService]timestamp:0x%x\r\n",timestamp);
	/*	1����ʱ���ת��Ϊʱ�䣬��ȡ��ʱ�����������ʱ����淶������Ƕ��ʽ��ʱ��������Դ洢ʱ����UTC+0���д洢	*/
	RTC_Timestamp_To_DateTime(timestamp,p_time);//��ʱ���ת��Ϊʱ��
	log_info("[dev_TimeService]tm_year:%d-",dev_time.tm_year);
	log_info("%d-",dev_time.tm_mon);
	log_info("%d ",dev_time.tm_mday);
	log_info("%d:",dev_time.tm_hour);
	log_info("%d:",dev_time.tm_min);
	log_info("%d\r\n",dev_time.tm_sec);
	
	/*	2����ʱ����д洢			*/
	RTC_SetDataTime(dev_time.tm_year,dev_time.tm_mon,dev_time.tm_mday,dev_time.tm_hour,dev_time.tm_min,dev_time.tm_sec);//����ʱ��			
	
	//HAL_Delay(3000);
	
	RTC_CalendarShow(&timestamp);
	
	HAL_IWDG_Refresh(&hiwdg);//ι��
}

/*******************************************************************************
* Function Name  : dev_Set_SampleInterval
* Description    : ���ò���ʱ������ͨ��ʱ�䴰��
* Input          : p_buf����������ָ�룬len���������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t dev_Set_SampleInterval(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint16_t checksum=0;
	
	uint32_t sample_interval=0;
	uint16_t receive_window=0;
	
	
	sample_interval=((p_buf[14]<<24) | (p_buf[15]<<16) | (p_buf[16]<<8) | p_buf[17]);
	receive_window=(p_buf[18]<<8) | p_buf[19];
	
	if((sample_interval<2) || (sample_interval>8640))return 1;
	if((receive_window<50) || (receive_window>5000))return 1;
	
	EEWrite(34,(void *)&sample_interval,4);	//�洢����
	EEWrite(38,(void *)&receive_window,2);		//�洢����
	Deviceinfo.sample_interval=sample_interval;			//����ȫ�ֱ���
	Deviceinfo.rx_window=receive_window;				//����ȫ�ֱ���
	//Set_AlarmA(Deviceinfo.sample_interval);				//����RTC����
	
	HAL_IWDG_Refresh(&hiwdg);//ι��
	
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x09;
	
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
	//log_info("time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	//HAL_Delay(20);
}

/*******************************************************************************
* Function Name  : dev_inquire_info
* Description    : �����豸��Ϣ
* Input          : p_buf����������ָ�룬len���������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void dev_inquire_info(uint8_t *p_buf,uint8_t len)
{
	
	uint32_t Sample_interval=0;	//�ɼ�ʱ������������ʱ��Ĭ��1h
	uint16_t Receive_Window=0;	//����ʱ�䴰�ڣ�������ʱ��Ĭ��100ms
	uint8_t SI_ack[64]={0};
	uint32_t timestamp=0;
	uint16_t battery_val=0;
	uint16_t total_worktime=0;
	uint16_t SoftwareVersion=0;
	uint16_t checksum=0,ix=1;
	
	
	RTC_CalendarShow(&timestamp);//��ȡʱ���
	Receive_Window=Deviceinfo.rx_window;
	battery_val=get_adc(0);
	total_worktime=Deviceinfo.work_time;
	SoftwareVersion=Deviceinfo.sv;

	//log_info("[dev_inquire_info]timestamp:%d,Receive_Window:%dms,battery_val:%.2f,total_worktime:%dh,SoftwareVersion:0x%x\r\n",\
				timestamp,Receive_Window,battery_val*0.15/1000,total_worktime,SoftwareVersion);
	//log_info("[dev_inquire_info]Storage_count:%d\r\n",Deviceinfo.Sample_count);
	
	HAL_IWDG_Refresh(&hiwdg);//ι��

	HAL_Delay(15);
	//���ͻ�ִ
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0xa0;
	SI_ack[3]=p_buf[3]+1;
	SI_ack[4]=0x09;
	
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
	//log_info("[dev_inquire_info]time_Ack\r\n");
	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	//HAL_Delay(15);
	
	HAL_IWDG_Refresh(&hiwdg);//ι��
	
	//�ϴ�����
	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0x80;
	SI_ack[3]=0;
	SI_ack[4]=0x15;
	
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
	
//	SI_ack[28]=0x00;
//	SI_ack[29]=0x00;
//	SI_ack[30]=0x00;
//	SI_ack[31]=0x00;
	
	log_info("checksum:0x%x,SI_ack[26]:0x%x,SI_ack[27]:0x%x\r\n",checksum,SI_ack[26],SI_ack[27]);
	
//	while(ix)
	//{
		SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,28);
		//HAL_Delay(1000);
//	}
	
	
}


void dev_Send_EOT(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint16_t checksum=0;
	

	SI_ack[0]=0x55;
	SI_ack[1]=0xaa;
	SI_ack[2]=0x80;
	SI_ack[3]=0;
	SI_ack[4]=0x09;
	
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

	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	
	//HAL_Delay(16);
	//ACK
	//log_info("time_Ack\r\n");

}

/*******************************************************************************
* Function Name  : dev_Extract_Ack
* Description    : ����ACK
* Input          : p_buf����������ָ�룬len���������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t dev_Extract_Ack(uint8_t *p_buf,uint8_t len)
{	
	uint8_t SI_ack[64]={0};
	uint32_t dID=0;
	uint8_t ack_nack=0;
	
	uint8_t	rx_len=0,i=0;
	uint16_t Frame_header=0;
	uint8_t  Frame_type=0;
	uint8_t  FrameData_len=0;
	
	uint16_t Frame_checksum=0,checksum=0;
	uint8_t  Instruction_code=0;
	uint8_t  err=0;		
	
	rx_len=len;
	for(i=0;i<rx_len;i++)
	{
		log_info("%x ",p_buf[i]);
	}
	log_info("\r\n");
	
	
	Frame_header=(p_buf[0]<<8)|p_buf[1];
	Frame_type=p_buf[2];
	FrameData_len=p_buf[4];
	Instruction_code=p_buf[5];
	dID=(p_buf[10]<<24) | (p_buf[11]<<16) | (p_buf[12]<<8) | p_buf[13];//��ȡĿ�ĵ�ַ
	
	Frame_checksum=(p_buf[rx_len-2]<<8) | p_buf[rx_len-1];
	checksum=Check_cumulative_sum(&p_buf[5],len-7);//�ж�У���
	
	
	if(Frame_header!=0x55aa)							err |= 0x02;//�ж�֡ͷ
	if(Frame_type!=0x20 )								err |= 0x04;//�����������ָ�����˳�
	if(dID != Deviceinfo.id)							err |= 0x08;//IDƥ��
	//if(checksum != Frame_checksum  )					err |= 0x10;//У��ʹ���
	if((Instruction_code<5) || (Instruction_code>6) )	err |= 0x20;//ָ�������
	
	log_info("Instruction_code:%d\r\n",Instruction_code);
	
	if(Instruction_code==5)
	{
		if(rx_len!=17)err |= 0x01;//�жϽ������ݵĳ���
	}
	else if(Instruction_code==6)
	{
		if(rx_len!=16)err |= 0x01;//�жϽ������ݵĳ���
	}
	
	if(err!=0)
	{
		log_info("err:%d\r\n",err);
		return 1;
	}
	
	if(Instruction_code==5)
	{
		ack_nack=p_buf[14];
		return ack_nack;
	}
	else if(Instruction_code==6)
	{
		ack_nack=Instruction_code;
		return ack_nack;
	}
}

/*******************************************************************************
* Function Name  : dev_data_request
* Description    : ��������
* Input          : p_buf����������ָ�룬len���������ݳ���
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
	uint16_t i=0,j=0,k=0,m=0;
	uint16_t checksum=0;
	uint8_t  send_frame_num=0;
	uint16_t timeout1=0,timeout2=0;
	uint32_t dID=0;
	uint8_t ack_nack=0;
	
	/* ���ͻ�ִ��Ϣ  */
	
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

	SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,16);
	//ACK
	//log_info("[dev_data_request]data_Ack\r\n");
	//HAL_Delay(16);
	HAL_IWDG_Refresh(&hiwdg);//ι��
	
	/*	
		���ݴ���
		һ֡���ݰ���8�����ݵ�
		1B����
		ÿ�����ݵ����5���ֽڣ�1B�¶�+4Bʱ���
		2BУ���
		
	*/
		
	data_count=Deviceinfo.Sample_count;				//��ȡ��ǰ��������
	
	if(data_count==0)return 1;						//�����ǰ�����ݣ�ֱ�ӷ��ػ򷵻ض�Ӧ��ACK
	
	page_count=(data_count-1)/32+1;					//�������һ���������ڵ�ҳ��
	in_page_num=(data_count-1)%32;					//�������һ���������ڵ����
	send_num=(data_count-1)/8+1;					//��������Ҫ���͵����ݰ�����
	
	log_info("data_count:%d,page_count:%d,in_page_num:%d,send_num:%d\r\n",data_count,page_count,in_page_num,send_num);
	//return 1;
	
	for(i=1;i<=page_count;i++)
	{
		AT45dbxx_ReadPage(flash_buf,264,i);			//����Ӧ��ҳ��ÿҳ32�����ݣ���Ϊ4����ÿ��8����
		HAL_IWDG_Refresh(&hiwdg);//ι��
		if(i!=page_count)							//����������һҳ����
		{
			for(j=0;j<4;j++)	//��Ϊ4������
			{
				SI_ack[0]=0x55;
				SI_ack[1]=0xaa;
				SI_ack[2]=0x80;
				SI_ack[3]=0;
				SI_ack[4]=0x32;
	
				SI_ack[5]=0x01;
				SI_ack[6]=p_buf[10];
				SI_ack[7]=p_buf[11];
				SI_ack[8]=p_buf[12];
				SI_ack[9]=p_buf[13];
	
				SI_ack[10]=p_buf[6];
				SI_ack[11]=p_buf[7];
				SI_ack[12]=p_buf[8];
				SI_ack[13]=p_buf[9];
				SI_ack[14]=send_frame_num;
				
				for(k=0;k<=7;k++)				//ÿ������8�����ݵ�
				{
					SI_ack[14+k*5+1]=2*((flash_buf[j*64+k*8+1])<<8|flash_buf[j*64+k*8+2])/100+40;		//ת��Ϊ���ֽ��¶�;
					SI_ack[14+k*5+2]=flash_buf[j*64+k*8+3];
					SI_ack[14+k*5+3]=flash_buf[j*64+k*8+4];
					SI_ack[14+k*5+4]=flash_buf[j*64+k*8+5];
					SI_ack[14+k*5+5]=flash_buf[j*64+k*8+6];
					if(SI_ack[14+k*5+2]==0xff)SI_ack[14+k*5+1]=0xff;
					log_info("\r\n");
					log_info("0x%x 0x%x 0x%x 0x%x 0x%x\r\n",SI_ack[14+k*5+1],SI_ack[14+k*5+2],SI_ack[14+k*5+3],SI_ack[14+k*5+4],SI_ack[14+k*5+5]);
				}

				checksum=Check_cumulative_sum(&SI_ack[5],50);
				log_info("checksum:0x%x\r\n",checksum);
				SI_ack[55]=(checksum&0xff00)>>8;
				SI_ack[56]=checksum&0xff;
	
		try_send_again1:	//������������ش���������5
				HAL_Delay(15);
				SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,32);
				HAL_Delay(15);
				
				SI446x_TX_RX_Data(0,(uint8_t *)&SI_ack[32],25);
				
				HAL_Delay(15);
				
				HAL_IWDG_Refresh(&hiwdg);//ι��
				
				//�ȴ�ACK,6s������Ӧ���˳�
				timeout1=3000;
				while(timeout1--)//��ʼ��ʱ100ms
				{
					SI446x_TX_RX_Data(1,wl_rx_buff,0);	//������������
					HAL_IWDG_Refresh(&hiwdg);//ι��
					if(wl_rx_Flag)	//������������ִ��ָ�����
					{	
						log_info("rx reply\r\n");
						ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//����ack����
						if(ack_nack==0)//�յ�ack,��������
						{
							log_info("rx ack---%d\r\n",ack_nack);
							send_frame_num++;//���ͳɹ���send_frame_num++
							clear_wl_rx_buff();
							break;
						}
						else if(ack_nack==1)//�յ�nack���ش�
						{
							log_info("rx nack---%d\r\n",ack_nack);
							clear_wl_rx_buff();
							goto try_send_again1;
						}
						else if(ack_nack==6)//�յ�can���ɼ�������ȡ�����ݴ���
						{
					send_EOT1:
							log_info("rx can---%d\r\n",ack_nack);
							dev_Send_EOT(wl_rx_buff,wl_rx_Len);//����EOT
							clear_wl_rx_buff();
							timeout2=5000;
							while(timeout2--)
							{
								SI446x_TX_RX_Data(1,wl_rx_buff,0);	//������������
								HAL_IWDG_Refresh(&hiwdg);//ι��
								if(wl_rx_Flag)
								{
									ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//����ack����
									clear_wl_rx_buff();
									if(ack_nack==1)goto send_EOT1;
									if(ack_nack==0)break;
								}
								HAL_Delay(2);
								if(timeout2==0)return 1;//�����ʱ���˳�����
							}
							
							return 1;
						}	
						
					}
					HAL_Delay(2);
					if(timeout1==0)return 1;//�����ʱ���˳�����
				}
				
				
			}
			
		}
		else					//���һҳ
		{
			for(j=0;j<in_page_num/8+1;j++)	//��Ϊin_page_num/8+1�����ͣ�in_page_num(0-31)
			{
				SI_ack[0]=0x55;
				SI_ack[1]=0xaa;
				SI_ack[2]=0x80;
				SI_ack[3]=0;
				SI_ack[4]=0x32;
	
				SI_ack[5]=0x01;
				SI_ack[6]=p_buf[10];
				SI_ack[7]=p_buf[11];
				SI_ack[8]=p_buf[12];
				SI_ack[9]=p_buf[13];
	
				SI_ack[10]=p_buf[6];
				SI_ack[11]=p_buf[7];
				SI_ack[12]=p_buf[8];
				SI_ack[13]=p_buf[9];
				SI_ack[14]=send_frame_num;
				
				HAL_IWDG_Refresh(&hiwdg);//ι��
				
				for(k=0;k<=7;k++)				//ÿ������8�����ݵ�
				{
					SI_ack[14+k*5+1]=2*((flash_buf[j*64+k*8+1])<<8|flash_buf[j*64+k*8+2])/100+40;		//ת��Ϊ���ֽ��¶�;
					SI_ack[14+k*5+2]=flash_buf[j*64+k*8+3];
					SI_ack[14+k*5+3]=flash_buf[j*64+k*8+4];
					SI_ack[14+k*5+4]=flash_buf[j*64+k*8+5];
					SI_ack[14+k*5+5]=flash_buf[j*64+k*8+6];
					if(SI_ack[14+k*5+2]==0xff)SI_ack[14+k*5+1]=0xff;
					log_info("\r\n");
					log_info("0x%x 0x%x 0x%x 0x%x 0x%x\r\n",SI_ack[14+k*5+1],SI_ack[14+k*5+2],SI_ack[14+k*5+3],SI_ack[14+k*5+4],SI_ack[14+k*5+5]);
				}

				checksum=Check_cumulative_sum(&SI_ack[5],50);
				log_info("checksum:0x%x\r\n",checksum);
				SI_ack[55]=(checksum&0xff00)>>8;
				SI_ack[56]=checksum&0xff;
				
				for(m=0;m<57;m++)log_info("%x ",SI_ack[m]);
				log_info("\r\n");
	
		try_send_again2:	//������������ش���������5
				HAL_Delay(15);
				SI446x_TX_RX_Data(0,(uint8_t *)SI_ack,32);
				HAL_Delay(15);
				
				SI446x_TX_RX_Data(0,(uint8_t *)&SI_ack[32],25);
				
				HAL_Delay(15);
				
				//HAL_Delay(60);
				
				HAL_IWDG_Refresh(&hiwdg);//ι��
				
				
				//�ȴ�ACK,6s������Ӧ���˳�
				timeout1=3000;
				while(timeout1--)//��ʼ��ʱ100ms
				{
					SI446x_TX_RX_Data(1,wl_rx_buff,0);	//������������
					
					HAL_IWDG_Refresh(&hiwdg);//ι��
					if(wl_rx_Flag)	//������������ִ��ָ�����
					{	
						log_info("rx reply\r\n");
						ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//����ack����
						if(ack_nack==0)//�յ�ack,��������
						{
							log_info("rx ack---%d\r\n",ack_nack);
							send_frame_num++;//���ͳɹ���send_frame_num++
							log_info("j=%d,in_page_num/8+1=%d",j,in_page_num/8+1);
							if(j==in_page_num/8){dev_Send_EOT(wl_rx_buff,wl_rx_Len);return 0;}//��������һ�����ݣ�����EOT���˳�
							clear_wl_rx_buff();
							break;
						}
						else if(ack_nack==1)//�յ�nack���ش�
						{
							log_info("rx nack---%d\r\n",ack_nack);
							clear_wl_rx_buff();
							goto try_send_again2;
						}
						else if(ack_nack==6)//�յ�can���ɼ�������ȡ�����ݴ���
						{
					send_EOT2:
							log_info("rx can---%d\r\n",ack_nack);
							dev_Send_EOT(wl_rx_buff,wl_rx_Len);//����EOT
							clear_wl_rx_buff();
							timeout2=5000;
							while(timeout2--)
							{
								SI446x_TX_RX_Data(1,wl_rx_buff,0);	//������������
								HAL_IWDG_Refresh(&hiwdg);//ι��
								if(wl_rx_Flag)
								{
									ack_nack=dev_Extract_Ack(wl_rx_buff,wl_rx_Len);//����ack����
									clear_wl_rx_buff();
									if(ack_nack==1)goto send_EOT2;
									if(ack_nack==0)break;
								}
								HAL_Delay(2);
								if(timeout2==0)return 1;//�����ʱ���˳�����
							}
							
							return 1;
						}	
						
					}
					HAL_Delay(2);
					if(timeout1==0)return 1;//�����ʱ���˳�����
				}
			}
		}
		
	}
}


uint8_t Instruction_Process(void)
{	
	uint8_t err=0,i=0;
	
	uint8_t data_buf[64]={0};
	
	uint8_t	rx_len=0;
	uint16_t Frame_header=0;
	uint8_t  Frame_type=0;
	uint32_t dID=0;
	uint16_t Frame_checksum=0,checksum=0;
	uint8_t  Instruction_code=0;
	
	rx_len=wl_rx_Len;
	memcpy(data_buf,wl_rx_buff,64);
	
	wl_rx_Flag=0;
	wl_rx_Len=0;
	memset(wl_rx_buff,0,64);	//�ͷŻ���
	
	for(i=0;i<rx_len;i++)
	{
		log_info("%x ",data_buf[i]);
	}
	log_info("\r\n");
	
	
	Frame_header=(data_buf[0]<<8)|data_buf[1];
	Frame_type=data_buf[2];
	dID=(data_buf[10]<<24) | (data_buf[11]<<16) | (data_buf[12]<<8) | data_buf[13];//��ȡĿ�ĵ�ַ
	
	Frame_checksum=(data_buf[rx_len-2]<<8) | data_buf[rx_len-1];
	checksum=Check_cumulative_sum(&data_buf[5],rx_len-7);//�ж�У���
	
	Instruction_code=data_buf[5];
	
	if(Frame_header!=0x55aa)							err |= 0x02;//�ж�֡ͷ
	if(Frame_type!=0x20 )								err |= 0x04;//�����������ָ�����˳�
	if(dID != Deviceinfo.id)							err |= 0x08;//IDƥ��
	//if(checksum != Frame_checksum  )					err |= 0x10;//У��ʹ���
	if((Instruction_code<1) || (Instruction_code>4) )	err |= 0x20;//ָ�������
	
	if(Instruction_code==1)
	{
		if(rx_len!=24)err |= 0x01;//�жϽ������ݵĳ���
	}
	else if(Instruction_code==2)
	{
		if(rx_len!=22)err |= 0x01;//�жϽ������ݵĳ���
	}
	else if(Instruction_code==3)
	{
		if(rx_len!=16)err |= 0x01;//�жϽ������ݵĳ���
	}
	log_info("data_buf[rx_len-2]:0x%x,data_buf[rx_len-1]:0x%x,\r\n",data_buf[rx_len-2],data_buf[rx_len-1]);
	log_info("Frame_header:0x%x,Frame_type:0x%x,dID:0x%x,checksum:0x%x,Instruction_code:0x%x,rx_len:0x%x,\r\n",Frame_header,Frame_type,dID,checksum,Instruction_code,rx_len);
	if(err!=0)
	{
		log_info("err:%d\r\n",err);
		return 1;
	}
	
		HAL_IWDG_Refresh(&hiwdg);//ι��
		switch(data_buf[5])
		{
			case time_service:			//��ʱ
						dev_TimeService(data_buf,rx_len);
						break;
			case set_sample_interval:	//���ò������
						dev_Set_SampleInterval(data_buf,rx_len);
						break;
			case inquire_info:			//�����豸��Ϣ
						dev_inquire_info(data_buf,rx_len);
						break;
			case data_request:			//��������
						dev_data_request(data_buf,rx_len);	//���յ���������ָ��
						break;
			default:
						break;
		}
	
}









