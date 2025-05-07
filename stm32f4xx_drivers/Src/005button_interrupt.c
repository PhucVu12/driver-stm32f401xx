/*
 * 005button_interrupt.c
 *
 *  Created on: Apr 10, 2025
 *      Author: phucv
 */

#include "stm32f401xx.h"
#include <stdint.h>
#include <string.h>

#define HIGH ENABLE
#define BTN_PRESSED HIGH

void delay(void)
{
	for(uint32_t i=0; i<500000; i++);
}

int main(void)
{
    GPIO_Handle_t GpioLed, GPIOBtn;

    memset(&GpioLed,0,sizeof(GpioLed));
    memset(&GPIOBtn,0,sizeof(GPIOBtn));
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
    GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
    GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOA,ENABLE);
	GPIO_Init(&GPIOBtn);

	//IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXIT9_5, NVIC_IRQ_PRIO15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXIT9_5, ENABLE);



	while(1)
    {

    }
	return 0;
}

void EXTI9_5_IRQHandler(void)
{
	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_5);
	GPIO_ToggleOutputPin(GPIOC, GPIO_PIN_NO_13);


}
