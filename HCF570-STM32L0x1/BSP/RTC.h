#ifndef __RTC_H
#define __RTC_H
#include <time.h>


extern RTC_HandleTypeDef hrtc;

unsigned char Is_Leap_Year(unsigned short int year);
unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day);
void MX_RTC_Init(void);
void RTC_SetDataTime(unsigned short int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);
void RTC_CalendarShow(void);

uint32_t RTC_DateTime_To_Timestamp(unsigned short int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec);
void RTC_Timestamp_To_DateTime(time_t timestamp);

unsigned char Set_Alarm(void);

#endif

