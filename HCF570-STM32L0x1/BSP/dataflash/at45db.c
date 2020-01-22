#include "main.h"
#include "at45db_Config.h"


/****************************************************************************** 
**Status Register Format:                                                      
**   ------------------------------------------------------------------------   
**  |  bit7  |  bit6  |  bit5  |  bit4  |  bit3  |  bit2  |  bit1  |  bit0   |  
**  |--------|--------|--------|--------|--------|--------|--------|-------- |  
**  |RDY/BUSY|  COMP  |   1    |   1    |   0    |   1    | PROTECT|PAGE SIZE|  
**   ------------------------------------------------------------------------   
**  bit7 - 忙标记，0为忙1为不忙。                                             
**         当Status Register的位0移出之后，接下来的时钟脉冲序列将使SPI器件继续 
**         将最新的状态字节送出。                                              
**  bit6 - 标记最近一次Main Memory Page和Buffer的比较结果，0相同，1不同。      
**  bit5                                                                       
**  bit4                                                                       
**  bit3                                                                       
**  bit2 - 这4位用来标记器件密度，对于AT45DB041B，这4位应该是，一共能标记  
**         16种不同密度的器件。                                                
**  bit1 - 保护标志位,1为已保护,0为未保护                                      
**  bit0 - 页大小设置,1为256byte,0为264byte                                    
**默认页面大小为528
******************************************************************************/ 


//################################################################################################################

AT45dbxx_t	AT45dbxx;

/*******************************************************************************
* Function Name  : AT45dbxx_Spi
* Description    : 通过SPI完成数据传输
* Input          : 要发送的数据
* Output         : None
* Return         : 接收到的数据
*******************************************************************************/
uint8_t	AT45dbxx_Spi(uint8_t Data)
{
	uint8_t ret=0;
	HAL_SPI_TransmitReceive(&_45DBXX_SPI,&Data,&ret,1,100);
	return ret;
}

/*******************************************************************************
* Function Name  : AT45dbxx_ReadStatus
* Description    : 获取AT45DBxx的状态寄存器
* Input          : None
* Output         : None
* Return         : 返回的状态值
*******************************************************************************/
uint8_t AT45dbxx_ReadStatus(void) 
{ 
	uint8_t	status	= 0;
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_RDSR);
	status = AT45dbxx_Spi(0x00);
	SPI_SELECT(SPI_FLASH,SET);
	return status; 
}

/*******************************************************************************
* Function Name  : AT45dbxx_WaitBusy
* Description    : AT45DBxx忙等待
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_WaitBusy(void)
{
	uint8_t	status;
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_RDSR);
	status = AT45dbxx_Spi(0x00);
	SPI_SELECT(SPI_FLASH,SET);
	while((status & 0x80) == 0)
	{
		_45DBXX_DELAY(1);
		SPI_SELECT(SPI_FLASH,RESET);
		AT45dbxx_Spi(AT45DB_RDSR);
		status = AT45dbxx_Spi(0x00);
		SPI_SELECT(SPI_FLASH,SET);
	}
}

/*******************************************************************************
* Function Name  : AT45dbxx_Resume
* Description    : AT45DBxx
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_Resume(void) 
{
	SPI_SELECT(SPI_FLASH,RESET);	
	AT45dbxx_Spi(AT45DB_RESUME);
	SPI_SELECT(SPI_FLASH,SET);
}

/*******************************************************************************
* Function Name  : AT45dbxx_PowerDown
* Description    : AT45DBxx powerdown
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_PowerDown(void) 
{
	SPI_SELECT(SPI_FLASH,RESET);	
	AT45dbxx_Spi(AT45DB_PWRDOWN);
	SPI_SELECT(SPI_FLASH,SET);	
}

/*******************************************************************************
* Function Name  : AT45dbxx_Init
* Description    : 初始化AT45DB
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t AT45dbxx_Init(void)
{
	uint8_t capacity=0;
	uint8_t devid[3]={0};
	
	SPI_SELECT(SPI_FLASH,SET);
	while(HAL_GetTick() < 20) _45DBXX_DELAY(10);
	
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_RDDEVID);
	devid[0]=AT45dbxx_Spi(0xa5);
	devid[1]=AT45dbxx_Spi(0xa5);
	SPI_SELECT(SPI_FLASH,SET);
	devid[2]=AT45dbxx_ReadStatus();
	log_info("AT45DB Status:0x%x\r\n",devid[2]);
	
	if(devid[0]==AT45DB_MANUFACTURER)
	{
		capacity=devid[1]&AT45DB_DEVID1_CAPMSK;
		
		switch	(capacity)
		{
			case AT45DB_DEVID1_2MBIT:	//	AT45db021
					AT45dbxx.FlashSize_MBit = 2;
					AT45dbxx.Pages = 1024;
					if(devid[2]&0x01)
					{
						AT45dbxx.Shift = 0;
						AT45dbxx.PageSize = 256;				
					}
					else
					{
						AT45dbxx.Shift = 9;
						AT45dbxx.PageSize = 264;				
					}
			break;
			case AT45DB_DEVID1_4MBIT:	//	AT45db041
				AT45dbxx.FlashSize_MBit = 4;
				AT45dbxx.Pages = 2048;
				log_info("AT45DB041\r\n");
				if(devid[2]&0x01)
				{
					AT45dbxx.Shift = 0;
					AT45dbxx.PageSize = 256;				
				}
				else
				{
					AT45dbxx.Shift = 9;
					AT45dbxx.PageSize = 264;				
				}
			break;
			case AT45DB_DEVID1_8MBIT:	//	AT45db081
				AT45dbxx.FlashSize_MBit = 8;
				AT45dbxx.Pages = 4096;
				if(devid[2]&0x01)
				{
					AT45dbxx.Shift = 0;
					AT45dbxx.PageSize = 256;				
				}
				else
				{
					AT45dbxx.Shift = 9;
					AT45dbxx.PageSize = 264;					
				}
			break;
			case AT45DB_DEVID1_16MBIT:	//	AT45db161
				AT45dbxx.FlashSize_MBit = 16;
				AT45dbxx.Pages = 4096;
				if(devid[2]&0x01)
				{
					AT45dbxx.Shift = 0;					
					AT45dbxx.PageSize = 512;				
				}
				else
				{
					AT45dbxx.Shift = 10;
					AT45dbxx.PageSize = 528;					
				}
			break;
			case AT45DB_DEVID1_32MBIT:	//	AT45db321
				AT45dbxx.FlashSize_MBit = 32;
				AT45dbxx.Pages = 8192;
				if(devid[2]&0x01)
				{
					AT45dbxx.Shift = 0;
					AT45dbxx.PageSize = 512;				
				}
				else
				{
					AT45dbxx.Shift = 10;
					AT45dbxx.PageSize = 528;					
				}
			break;
			case AT45DB_DEVID1_64MBIT:	//	AT45db641
				AT45dbxx.FlashSize_MBit = 64;
				AT45dbxx.Pages = 8192;
				if(devid[2]&0x01)
				{
					AT45dbxx.Shift = 0;
					AT45dbxx.PageSize = 1024;				
				}
				else
				{
					AT45dbxx.Shift = 11;
					AT45dbxx.PageSize = 1056;				
				}
			break;			
		}			
		
		return true;
	}
	else
		return false;	
}

/*******************************************************************************
* Function Name  : AT45dbxx_EraseChip
* Description    : 擦除整片芯片
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_EraseChip(void)
{
	AT45dbxx_Resume();
	AT45dbxx_WaitBusy();
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_CHIPERASE1);
	AT45dbxx_Spi(AT45DB_CHIPERASE2);
	AT45dbxx_Spi(AT45DB_CHIPERASE3);
	AT45dbxx_Spi(AT45DB_CHIPERASE4);
	SPI_SELECT(SPI_FLASH,SET);	
	AT45dbxx_WaitBusy();	
}

/*******************************************************************************
* Function Name  : AT45dbxx_ErasePage
* Description    : 擦除芯片的一页
* Input          : page地址
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_ErasePage(uint16_t page)
{
	page = page << AT45dbxx.Shift;
	AT45dbxx_Resume();
	AT45dbxx_WaitBusy();
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_PGERASE);
	AT45dbxx_Spi((page >> 16) & 0xff);
	AT45dbxx_Spi((page >> 8) & 0xff);
	AT45dbxx_Spi(page & 0xff);
	SPI_SELECT(SPI_FLASH,SET);
	AT45dbxx_WaitBusy();
}

/*******************************************************************************
* Function Name  : AT45dbxx_WritePage
* Description    : 写其中的一页
* Input          : 数据指针，数据长度，page地址
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_WritePage(uint8_t	*Data,uint16_t len,uint16_t	page)
{
	page = page << AT45dbxx.Shift;
	AT45dbxx_Resume();
	AT45dbxx_WaitBusy();
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_MNTHRUBF1);
	AT45dbxx_Spi((page >> 16) & 0xff);
	AT45dbxx_Spi((page >> 8) & 0xff);
	AT45dbxx_Spi(page & 0xff);
	HAL_SPI_Transmit(&_45DBXX_SPI,Data,len,100);
	SPI_SELECT(SPI_FLASH,SET);
	AT45dbxx_WaitBusy();	
}

/*******************************************************************************
* Function Name  : AT45dbxx_ReadPage
* Description    : 读其中的一页
* Input          : 数据指针，数据长度，page地址
* Output         : None
* Return         : None
*******************************************************************************/
void AT45dbxx_ReadPage(uint8_t* Data,uint16_t len, uint16_t page)
{	
	page = page << AT45dbxx.Shift;
	if(len > AT45dbxx.PageSize)
		len = AT45dbxx.PageSize;
	AT45dbxx_Resume();
	AT45dbxx_WaitBusy();
	SPI_SELECT(SPI_FLASH,RESET);
	AT45dbxx_Spi(AT45DB_RDARRAYHF);
	AT45dbxx_Spi((page >> 16) & 0xff);
	AT45dbxx_Spi((page >> 8) & 0xff);
	AT45dbxx_Spi(page & 0xff);
	AT45dbxx_Spi(0);			
	HAL_SPI_Receive(&_45DBXX_SPI,Data,len,100);	
	SPI_SELECT(SPI_FLASH,SET);
}


void AT45_test(void)
{
	unsigned char test[264]={0};
	unsigned char test1[264]={0};
	unsigned short i=0;
	
	for(i=0;i<255;i++)
				test1[i]=i;
	
	AT45dbxx_WritePage(test1,264,1);
	AT45dbxx_WritePage(test1,264,2);
	
	//AT45dbxx_ErasePage(1);
	
	AT45dbxx_ReadPage(test,264,1);
	
	for(i=0;i<32;i++)
		log_info("0x%x ",test[i]);
	log_info("\r\n");
	
	
	
	
	//AT45dbxx_ErasePage(1);
	
	AT45dbxx_ReadPage(test,264,2);
	
	for(i=0;i<32;i++)
		log_info("0x%x ",test[i]);
	log_info("\r\n");
	
	
}



