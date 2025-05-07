/*
 * 007_spi_txonly_arduino.c
 *
 *  Created on: Apr 14, 2025
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

void delay(void)
{
	for(uint32_t i=0; i<500000; i++);
}

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
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPI2handle);
}

void GPIO_ButtonInits(void)
{
	GPIO_Handle_t GPIOBtn;
	 //BUTTON PA1
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;


	GPIO_Init(&GPIOBtn);
}

int main()
{
	//user data to send
	char user_data[] = "Hello world";

	//SPI2 Pin init
	SPI2_GPIOInits();

	//SPI2 init
	SPI2_Inits();

	//Button init
	GPIO_ButtonInits();

	/*
	 * Making SSOE 1 does NSS output enable
	 * The NSS pin is automatically managed by the hardware
	 * When SPI = 1 then NSS will be pulled to Low
	 * and NSS pin will be high when SPE = 0*/
	SPI_SSOEConfig(SPI2, ENABLE);
	while(1)
	{
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();
		//enable the SPI peripheral
		SPI_PeripheralControl(SPI2, ENABLE);

		//First send length info
		uint8_t dataLen = strlen(user_data);
		SPI_SendData(SPI2, &dataLen, 1);

		//Test TX send "Hello world"
		SPI_SendData(SPI2, (uint8_t*) user_data, strlen(user_data));

		//confirm SPI is not busy
		while( SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG) );

		//disable the SPI peripheral when it's done
		SPI_PeripheralControl(SPI2, DISABLE);
	}


	return 0;
}


