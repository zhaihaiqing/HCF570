#ifndef __SPI_H
#define __SPI_H
#include "stm32l0xx_hal_spi.h"

#define SPIFLASH_CS_L()	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,0)
#define SPIFLASH_CS_H()	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,1)

#define SPIAS10_CS_L()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,0)
#define SPIAS10_CS_H()	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,1)

enum
{
	SPI_FLASH=1,
	SPI_AS10
};

enum spi_mode_e
{
  SPIDEV_MODE0 = 0,   /* CPOL=0 CHPHA=0 */
  SPIDEV_MODE1,       /* CPOL=0 CHPHA=1 */
  SPIDEV_MODE2,       /* CPOL=1 CHPHA=0 */
  SPIDEV_MODE3        /* CPOL=1 CHPHA=1 */
};

extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void);

void SPI_SELECT(uint8_t spidev,uint8_t sta);

#endif

