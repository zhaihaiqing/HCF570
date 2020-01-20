/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define EEPROM_START_ADDR 	0x08080000

void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	while(length--)
	{
		*pbuff++=*waddr++;
	}
}

void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned int waddr=0;
	
	waddr=EEPROM_START_ADDR+addr;
	
	HAL_FLASHEx_DATAEEPROM_Unlock();
	while(length--)
	{
		
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,waddr++,*pbuff++); 
		
	}
    HAL_FLASHEx_DATAEEPROM_Lock();
}

