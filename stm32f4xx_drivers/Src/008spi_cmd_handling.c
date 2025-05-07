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
#include <stdio.h>
#include "stm32f401xx.h"
#include <string.h>

extern void initialise_monitor_handles(void);

//command codes
#define COMMAND_LED_CTRL			0x50
#define COMMAND_SENSOR_READ			0x51
#define COMMAND_LED_READ			0x52
#define COMMAND_PRINT				0x53
#define COMMAND_ID_READ				0x54

#define LED_ON		1
#define LED_OFF		0

// arduino analog pins
#define ANALOG_PIN0		0
#define ANALOG_PIN1		1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4

// arduino led
#define LED_PIN		9

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
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

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

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if(ackbyte == 0xF5)
	{
		//ack
		return 1;
	}else
	{
		//nack
		return 0;
	}
}

int main()
{
	initialise_monitor_handles();

	printf("Application is running\n");

	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;
	//SPI2 Pin init
	SPI2_GPIOInits();

	//SPI2 init
	SPI2_Inits();

	printf("SPI Init done\n");

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
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();
		//enable the SPI peripheral
		SPI_PeripheralControl(SPI2, ENABLE);

		//1. CMD_LED_CTRL <pin no(1)>	<value(1)>
		uint8_t commandcode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];
		SPI_SendData(SPI2, &commandcode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read , 1);

		//send some dummy bits(1byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackbyte , 1);

		if (SPI_VerifyResponse(ackbyte))
		{
			//send arguments
			args[0] = LED_PIN;
			args[1] = LED_ON;
			SPI_SendData(SPI2, args , 2);
			printf("COMMAND_LED_CTRL Executed\n");

		}


		//2. CMD_SENSOR_READ
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();

		commandcode = COMMAND_SENSOR_READ;

		//send cmt
		SPI_SendData(SPI2, &commandcode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read , 1);

		//send some dummy bits(1byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackbyte , 1);

		if (SPI_VerifyResponse(ackbyte))
		{
			//send arguments
			args[0] = ANALOG_PIN0;

			SPI_SendData(SPI2, args , 1);

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read , 1);

			//insert some delay so that slave can ready with data
			delay();

			//send some dummy bits(1byte) to fetch the response from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;
			SPI_ReceiveData(SPI2, &analog_read , 1);
			printf("COMMAND_SENSOR_READ %d\n", analog_read );
		}

		//3. CMD_LED_READ
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();
		commandcode = COMMAND_LED_READ;

		//send cmt
		SPI_SendData(SPI2, &commandcode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read , 1);

		//send some dummy bits(1byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackbyte , 1);

		if (SPI_VerifyResponse(ackbyte))
		{
			args[0] = LED_PIN;

			SPI_SendData(SPI2, args , 1);

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read , 1);

			//insert some delay so that slave can ready with data
			delay();

			//send some dummy bits(1byte) to fetch the response from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t led_status;
			SPI_ReceiveData(SPI2, &led_status , 1);
			printf("COMMAND_READ_LED %d\n", led_status );
		}

		//4. CMD_PRINT		<len(2)>  <message(len)>
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();
		commandcode = COMMAND_PRINT;

		//send cmt
		SPI_SendData(SPI2, &commandcode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read , 1);

		//send some dummy bits(1byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackbyte , 1);
		uint8_t message[] = "Hello ! How are you ??";
		if (SPI_VerifyResponse(ackbyte))
		{
			args[0] = strlen( (char*) message);

			//send arguments
			SPI_SendData(SPI2, args , 1);

			//send message
			SPI_SendData(SPI2, message, args[0]);
			printf("COMMAND_PRINT Executed \n");

		}

		//5.CMD_ID_READ
		//wait till button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));
		delay();
		commandcode = COMMAND_ID_READ;

		//send cmt
		SPI_SendData(SPI2, &commandcode, 1);

		//do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read , 1);

		//send some dummy bits(1byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		//read the ack byte received
		SPI_ReceiveData(SPI2, &ackbyte , 1);
		uint8_t id[10];
		uint32_t i = 10;
		if (SPI_VerifyResponse(ackbyte))
		{
			for( i = 0; i < 10; i++ )
			{
				//send dummy byte to fetch data from slave
				SPI_SendData(SPI2, &dummy_write, 1);
				SPI_ReceiveData(SPI2, &id[i], 1);
			}

			id[11] = '\0';
			printf("COMMAND_ID : %s\n", id);
		}








		//confirm SPI is not busy
		while( SPI_GetFlagStatus(SPI2,SPI_BUSY_FLAG) );

		//disable the SPI peripheral when it's done
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("SPI communication Closed\n");
	}


	return 0;
}



