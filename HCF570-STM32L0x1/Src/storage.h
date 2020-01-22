#ifndef __STORAGE_H
#define __STORAGE_H
#include "stm32l031xx.h"


typedef struct __attribute__ ((__packed__))
{
	uint16_t id;					//�豸ID
	uint16_t sv;					//����汾��
	uint32_t dataamount;			//�Ѵ洢��������
	uint32_t start_work_timestamp;  //��ʼ����ʱ��ʱ���
	uint16_t work_time;				//����ʱ��
	int16_t temp_c;					//��ǰ�豸�¶�
	uint16_t bat_v;					//��ǰ��ص�ѹ
}Deviceinfo_type;



typedef struct __attribute__ ((__packed__))
{
	uint8_t iddet1;					//��ʶ��1 "#",0x23
	uint8_t iddet2;					//��ʶ��2 "#",0x23
	uint8_t temp;					//�¶�
	uint32_t timestamp;				//unixʱ���
	uint8_t  sumcheck;				//У��ͣ��ۼӺ�
}Datastorage_type;


extern Datastorage_type 	Datastorage;
extern Deviceinfo_type     Deviceinfo;


#endif

