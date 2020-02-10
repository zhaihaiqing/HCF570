#ifndef __STORAGE_H
#define __STORAGE_H
#include "stm32l031xx.h"


typedef struct __attribute__ ((__packed__))
{
	uint32_t Sample_count;			//�Ѵ洢��������
	uint32_t id;					//�豸ID
	uint16_t dev_ty;				//�豸����
	uint16_t sv;					//����汾��		
	uint16_t work_time;				//����ʱ����Сʱ��
	int16_t temp_c;					//��ǰ�豸�¶�
	uint16_t bat_v;					//��ǰ��ص�ѹ
	uint32_t sample_interval;		//���ݲɼ����
	uint16_t rx_window;				//����ʱ�䴰��
}Deviceinfo_type;



typedef struct __attribute__ ((__packed__))
{
	uint8_t iddet1;					//��ʶ��1 "#",0x23
	short temp;						//�¶�
	uint32_t timestamp;				//unixʱ���
	uint8_t  sumcheck;				//У��ͣ��ۼӺ�
}Datastorage_type;


extern Datastorage_type 	Datastorage;
extern Deviceinfo_type     Deviceinfo;


void dev_data_sample_and_storage(void);

#endif

