#ifndef __STORAGE_H
#define __STORAGE_H
#include "stm32l031xx.h"


typedef struct __attribute__ ((__packed__))
{
	uint32_t Sample_count;			//已存储的数据量
	uint32_t id;					//设备ID
	uint16_t dev_ty;				//设备类型
	uint16_t sv;					//软件版本号		
	uint16_t work_time;				//工作时长（小时）
	int16_t temp_c;					//当前设备温度
	uint16_t bat_v;					//当前电池电压
	uint32_t sample_interval;		//数据采集间隔
	uint16_t rx_window;				//接收时间窗口
}Deviceinfo_type;



typedef struct __attribute__ ((__packed__))
{
	uint8_t iddet1;					//标识符1 "#",0x23
	short temp;						//温度
	uint32_t timestamp;				//unix时间戳
	uint8_t  sumcheck;				//校验和，累加和
}Datastorage_type;


extern Datastorage_type 	Datastorage;
extern Deviceinfo_type     Deviceinfo;


void dev_data_sample_and_storage(void);

#endif

