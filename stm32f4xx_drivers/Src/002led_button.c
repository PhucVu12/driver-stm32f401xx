/*
 * 002led_button.c
 *
 *  Created on: Apr 8, 2025
 *      Author: phucv
 */

#include "stm32f401xx.h"
#include <stdint.h>
#define HIGH ENABLE
#define BTN_PRESSED HIGH

void delay(void)
{
	for(uint32_t i=0; i<500000; i++);
}

int main(void)
{
    GPIO_Handle_t GpioLed, GPIOBtn;

    //LED PC13
    GpioLed.pGPIOx = GPIOC;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(GPIOC,ENABLE);
    GPIO_Init(&GpioLed);

    //BUTTON PA1
    GPIOBtn.pGPIOx = GPIOA;
    GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
    GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA,ENABLE);
	GPIO_Init(&GPIOBtn);

	while(1)
    {
		if (GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1) == BTN_PRESSED)
		{
			delay();
			GPIO_ToggleOutputPin(GPIOC, GPIO_PIN_NO_13);
		}
    }
	return 0;
}
