/*
 * 006spi_tx_testing.c
 *
 *  Created on: Apr 12, 2025
 *      Author: phucv
 */

/*
 * PB15 -> SPI2_MOSI
 * PB14 -> SPI2_MISO
 * PB13 -> SPI2_SCK
 * PB12 -> SPI2_NSS
 * Alternate function mode: 5
 */

#include "stm32f401xx.h"
#include <string.h>

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	//MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);

	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	SPI_Init(&SPI2handle);
}
int main()
{
	//user data to send
	char user_data[] = "Hello world";

	//SPI2 Pin init
	SPI2_GPIOInits();

	//SPI2 init
	SPI2_Inits();

	//This makes NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI2, ENABLE);

	//enable the SPI peripheral
	SPI_PeripheralControl(SPI2, ENABLE);

	//Test TX send "Hello world"
	SPI_SendData(SPI2, (uint8_t*) user_data, strlen(user_data));

	//confirm SPI is not busy
	while( SPI_GetFlagStatus(SPI2,SPI_SR_BSY) );

	//disable the SPI peripheral when it's done
	SPI_PeripheralControl(SPI2, DISABLE);
	while(1);

	return 0;
}

