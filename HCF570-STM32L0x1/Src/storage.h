#ifndef __STORAGE_H
#define __STORAGE_H
#include "stm32l031xx.h"


typedef struct __attribute__ ((__packed__))
{
	uint16_t id;					//设备ID
	uint16_t sv;					//软件版本号
	uint32_t dataamount;			//已存储的数据量
	uint32_t start_work_timestamp;  //开始工作时的时间戳
	
}Deviceinfo_type;



typedef struct __attribute__ ((__packed__))
{
	uint8_t iddet1;					//标识符1 "$",0x24
	uint32_t timestamp;				//unix时间戳
	uint16_t temp;					//温度
	uint8_t  sumcheck;				//校验和
}Datastorage_type;


extern Datastorage_type 	Datastorage;
extern Deviceinfo_type     Deviceinfo;


#endif

