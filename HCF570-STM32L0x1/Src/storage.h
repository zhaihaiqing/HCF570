#ifndef __STORAGE_H
#define __STORAGE_H
#include "stm32l031xx.h"


typedef struct __attribute__ ((__packed__))
{
	uint16_t id;					//�豸ID
	uint16_t sv;					//����汾��
	uint32_t dataamount;			//�Ѵ洢��������
	uint32_t start_work_timestamp;  //��ʼ����ʱ��ʱ���
	
}Deviceinfo_type;



typedef struct __attribute__ ((__packed__))
{
	uint8_t iddet1;					//��ʶ��1 "$",0x24
	uint32_t timestamp;				//unixʱ���
	uint16_t temp;					//�¶�
	uint8_t  sumcheck;				//У���
}Datastorage_type;


extern Datastorage_type 	Datastorage;
extern Deviceinfo_type     Deviceinfo;


#endif

