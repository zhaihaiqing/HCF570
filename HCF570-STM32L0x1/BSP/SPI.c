/* Includes ------------------------------------------------------------------*/
#include "main.h"

SPI_HandleTypeDef hspi1;

/*******************************************************************************
* Function Name  : MX_SPI1_Init
* Description    : SPI初始换函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MX_SPI1_Init(void)
{
  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

/*******************************************************************************
* Function Name  : SPI_SELECT
* Description    : SPI 片选信号选择函数
* Input          : spidev:器件选择,sta:状态
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_SELECT(uint8_t spidev,uint8_t sta)
{
	switch(spidev)
	{
		case SPI_FLASH:
			if(sta==0) SPIFLASH_CS_L();
			else	   SPIFLASH_CS_H();
			break;
		case SPI_AS10:
			if(sta==0) SPIAS10_CS_L();
			else	   SPIAS10_CS_H();
			break;
		default:
			break;
	}
}



void SPI_ReadOnePage()
{
	
}



