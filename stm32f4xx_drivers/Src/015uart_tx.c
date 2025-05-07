/*
 * 015uart_tx.c
 *
 *  Created on: May 6, 2025
 *      Author: phucv
 */

#include <stdio.h>
#include <string.h>
#include "stm32f401xx.h"

char msg[1024] = "UART Tx Testing...\n\r";

void delay(void)
{
	for(uint32_t i=0; i<500000; i++);
}

USART_Handle_t usart2_handle;

void USART2_Init(void)
{
	usart2_handle.pUSARTx = USART2;
	usart2_handle.USART_Config_t.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config_t.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config_t.USART_Mode = USART_MODE_ONLY_TX;
	usart2_handle.USART_Config_t.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config_t.USART_WordLength = USART_WORDLEN_8BITS;
	usart2_handle.USART_Config_t.USART_ParityControl = USART_PARITY_DISABLE;
	USART_Init(&usart2_handle);

}

void USART2_GPIOInit(void)
{
	GPIO_Handle_t USART_GPIOS;

	USART_GPIOS.pGPIOx = GPIOA;
	USART_GPIOS.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	USART_GPIOS.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	USART_GPIOS.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	USART_GPIOS.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	USART_GPIOS.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

	//USART2 TX
	USART_GPIOS.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&USART_GPIOS);

	//USART2 RX
	USART_GPIOS.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&USART_GPIOS);

}

void GPIO_ButtonInits(void)
{
	GPIO_Handle_t GPIOBtn;
	 //BUTTON PA1
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}

int main()
{
	GPIO_ButtonInits();

	USART2_GPIOInit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);

	while(1)
	{
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		delay();

		USART_SendData(&usart2_handle, (uint8_t*)msg, strlen(msg));
	}


	return 0;

}




