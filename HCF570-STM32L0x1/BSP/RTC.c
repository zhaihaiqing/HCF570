/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <time.h>

//#define 	GMT		0
//#define 	CST		(GMT+8)	

RTC_HandleTypeDef hrtc;

unsigned char const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5};//月修正数据表，平年的月份日期表
unsigned char const mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//

//static	uint8_t	 BCD2DEC(uint8_t bcd)
//{
//	return (bcd-(bcd>>4)*6);
//}

//static	uint8_t	 DEC2BCD(uint8_t dec)
//{
//	return (dec+(dec/10)*6);
//}


/*******************************************************************************
* Function Name  : Is_Leap_Year
* Description    : 判断是否是闰年
* Input          : year
* Output         : None
* Return         : 1,是闰年，0不是闰年
*******************************************************************************/
unsigned char Is_Leap_Year(unsigned short int year)
{
	if( ( (year%4==0) && (year%100!=0) ) || (year%400==0) )return 1;
	else return 0;
}

/*******************************************************************************
* Function Name  : RTC_Get_Week
* Description    : 计算某日的星期数
* Input          : 年，月，日
* Output         : None
* Return         : 1:周一，2:周二....
*******************************************************************************/
unsigned char RTC_Get_Week(unsigned short int year,unsigned char month,unsigned char day)
{
	int c=0;
	int y=0;
	int week=0;
	if(month==1 || month==2)
	{
		year--;
		month+=12;
	}
	c=year/100;
	y=year-c*100;
	week=(c/4)-2*c+(y+y/4)+(13*(month+1)/5)+day-1;
	while(week<0)week+=7;
	week%=7;
	if(week==0)week=7;
	return week;
}

/*******************************************************************************
* Function Name  : MX_RTC_Init
* Description    : RTC Initialization Function
* Input          : N
* Output         : None
* Return         : None 
*******************************************************************************/
void MX_RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
	
  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_SetDataTime(2020,1,1,0,0,0);
  }

}

/*******************************************************************************
* Function Name  : RTC_SetDataTime
* Description    : 设置RTC时间
* Input          : year mon day hour min sec
* Output         : None
* Return         : None 
*******************************************************************************/
void RTC_SetDataTime(unsigned short int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  uint8_t sweek=0;
  
  sweek=RTC_Get_Week(syear,smon,sday);
  syear=RTC_ByteToBcd2(syear%100);
  smon=RTC_ByteToBcd2(smon);
  sday=RTC_ByteToBcd2(sday);
  hour=RTC_ByteToBcd2(hour);
  min=RTC_ByteToBcd2(min);
  sec=RTC_ByteToBcd2(sec);

  /** Initialize RTC and set the Time and Date  */
  /*##-1- Configure the Time #################################################*/
  /* Set Time: 00:00:00 */
  sTime.Hours = hour;
  sTime.Minutes = min;
  sTime.Seconds = sec;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  
  /*##-2- Configure the Date #################################################*/
  /* Set Date: Wednesday January 1th 2020 */
  sDate.WeekDay = sweek;
  sDate.Month = smon;
  sDate.Date = sday;
  sDate.Year = syear;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  
  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);

}


/*******************************************************************************
* Function Name  : Set_Alarm
* Description    : 设置闹钟时间，在当前时间基础上+N分钟
* Input          : N
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char Set_Alarm(void)
{
	RTC_AlarmTypeDef sAlarm;
	
	/** Enable the Alarm A */			//闹钟A为每小时醒来一次，每次醒来执行数据采样
	
	sAlarm.AlarmTime.Hours = 0x21;
	sAlarm.AlarmTime.Minutes = 0x35;
	sAlarm.AlarmTime.Seconds = 0x30;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.TimeFormat=RTC_HOURFORMAT12_AM;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = (uint32_t)(RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES);						//按分钟进行闹钟
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x01;
	sAlarm.Alarm = RTC_ALARM_A;
	
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler();
	}

	/** Enable the WakeUp */			//RTC_WakeUp每10s醒来一次，每次醒来执行100ms联网
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 20480, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		Error_Handler();
	}
	return 0;
}

/*******************************************************************************
* Function Name  : RTC_CalendarShow
* Description    : 通过串口打印当前时间
* Input          : N
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_CalendarShow(void)
{
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;
	uint32_t timestamp=0;

	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
	
	timestamp=RTC_DateTime_To_Timestamp(2000 + sdatestructureget.Year,sdatestructureget.Month,sdatestructureget.Date,\
						stimestructureget.Hours,stimestructureget.Minutes,stimestructureget.Seconds);
	
	/* Display date Format : mm-dd-yy */
	log_info("date:%d-%d-%d ", 2000 + sdatestructureget.Year,sdatestructureget.Month, sdatestructureget.Date);
	/* Display time Format : hh:mm:ss */
	log_info("%d:%d:%d timestamp:%d\r\n", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds,timestamp);
	
	//RTC_Timestamp_To_DateTime(timestamp);
}

/*******************************************************************************
* Function Name  : RTC_GetTimestamp
* Description    : 将当前时间转换成对应的unix时间戳
* Input          : N
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t RTC_DateTime_To_Timestamp(unsigned short int syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	uint32_t timestamp=0;
	struct tm	stm;
	
	stm.tm_year = syear-1900;
	stm.tm_mon  =smon-1;
	stm.tm_mday	=sday;
	stm.tm_hour= hour-8;
	stm.tm_min=min;
	stm.tm_sec=sec;
	
	timestamp=mktime(&stm);
	
	return timestamp;
}


/*******************************************************************************
* Function Name  : RTC_GetTimestamp
* Description    : 将当前时间转换成对应的unix时间戳
* Input          : N
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Timestamp_To_DateTime(time_t timestamp)
{
	struct tm *stm;
	int syear=0;
	int smon=0;
	int sday=0;
	int hour=0;
	int min=0;
	int sec=0;
	
	timestamp=timestamp+8*3600;//UTC+8
	
	stm=localtime(&timestamp);
	
	syear=stm->tm_year+1900;
	smon=stm->tm_mon+1;
	sday=stm->tm_mday;
	hour=stm->tm_hour;
	min=stm->tm_min;
	sec=stm->tm_sec;
	
	log_info("timestamp:%d,date:%d-%d-%d %d:%d:%d\r\n",  timestamp,syear,smon,sday,hour,min,sec);
}

/*******************************************************************************
* Function Name  : HAL_RTC_AlarmAEventCallback
* Description    : 闹钟A的中断服务函数（回调）
* Input          : hrtc
* Output         : None
* Return         : None
*******************************************************************************/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	alarm_flag=1;
}

/*******************************************************************************
* Function Name  : HAL_RTCEx_WakeUpTimerEventCallback
* Description    : rtc的唤醒中断服务函数
* Input          : hrtc
* Output         : None
* Return         : None
*******************************************************************************/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	wakeup_flag=1;
}






