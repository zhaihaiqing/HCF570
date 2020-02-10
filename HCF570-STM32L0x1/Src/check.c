/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "check.h"


unsigned short Check_cumulative_sum(unsigned char *buf,unsigned char len)
{
	unsigned char i=0;
	unsigned short val=0;
	
	for(i=0;i<len;i++)
		val+=*buf++;
	
	return val;
}


unsigned char Check_Sum8(unsigned char *buf, unsigned int nword)
{
	unsigned int sum = 0;
	while( nword > 0 ) 
	{
		sum =sum+*buf;
		buf += 1;
		nword -= 1;
	}
	while (sum>>8)
	sum = (sum & 0xff) + (sum >> 8);

	return (char)~sum;
}

unsigned short Check_Sum16(unsigned short *buf, int nword)
{
	unsigned long sum;
 
	for(sum = 0; nword > 0; nword--)
	sum += *buf++;
 
	sum = (sum>>16) + (sum&0xffff);
	sum += (sum>>16);
 
	return ~sum;
}


/*******************************************************************************
* Function Name  : CRC16_Check
* Description    : CRC校验
* Input          : 数据指针，数据长度
* Output         : None
* Return         : CRC检验结果，注意大小端
*******************************************************************************/
unsigned short CRC16_Check(unsigned char *Pushdata,unsigned char length)
{
	uint16_t Reg_CRC=0xffff;
	uint8_t i,j;
	for( i = 0; i<length; i ++)
	{
		Reg_CRC^= *Pushdata++;
		for (j = 0; j<8; j++)
		{
			if (Reg_CRC & 0x0001)

			Reg_CRC=Reg_CRC>>1^0xA001;
			else
			Reg_CRC >>=1;
		}
	}
	return   Reg_CRC;
}


uint16_t check_t[3]={0};
void check_test(void)
{
	check_t[0]=0x55;
	check_t[1]=0xaa;
	check_t[2]=0x20;
	
	log_info("check_test:0x%x",Check_Sum16(check_t,3));
	
	
}





