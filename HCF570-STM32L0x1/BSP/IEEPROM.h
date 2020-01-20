#ifndef __IEEPROM_H
#define __IEEPROM_H



void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length);

#endif

