/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*
stm32L031低功耗模式共有以下几种模式：
*	低功耗运行模式
*	睡眠（立即睡眠或退出时睡眠）
*	低功耗睡眠（立即睡眠或退出时睡眠）
*	停止模式
*	待机模式
*/

void enter_stopmode(void)
{	
	log_info("enter stop mode!\r\n");
	
	HAL_PWREx_EnableUltraLowPower();				 /* Enable Ultra low power mode */
	HAL_PWREx_EnableFastWakeUp();					 /* Enable Fast WakeUP */
	
	
	MX_GPIO_Deinit();
	
	__HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);	/* Select MSI as system clock source after Wake Up from Stop mode */
	
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFI);	/* Enter STOP Mode */
	
	
	
	HAL_Init();
	HAL_Delay(10);
	Dev_ReInit();
	log_info("cpu wake up!\r\n");
	 
	
	
}





