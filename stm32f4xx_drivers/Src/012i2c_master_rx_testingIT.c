/*
 * 012i2c_master_rx_testingIT.c
 *
 *  Created on: Apr 20, 2025
 *      Author: phucv
 */

#include <stdio.h>
#include "stm32f401xx.h"
#include <string.h>

//extern void initialise_monitorhandles();

//Flag variable
uint8_t rxComplt = RESET;

//define device address
#define MY_ADDR 0x61
//define Slave address
#define SLAVE_ADDR 0x68


void delay(void)
{
	for(uint32_t i=0; i<500000; i++);
}

I2C_Handle_t I2C1Handle;

//Rcv buffer
uint8_t rcv_buf[32];

/*
 * PB6 -> SCL
 * PB7 -> SDA
 */
void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	//sda
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);

}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);

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

int main(void)
{
	//command
	uint8_t commandcode;

	uint8_t len;

	//initialise_monitor_handles();
	//printf("Application is running\n");

	//Button init
	GPIO_ButtonInits();

	//Configure the I2C1 GPIO pin
	I2C1_GPIOInits();

	//I2C1 peripheral configuration
	I2C1_Inits();

	//I2C IRQ Configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	//Enable I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);



	//ACK bit is made 1 after PE=1;
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);
	//*************************************



	//wait for button press
	while(1)
	{
		//wait till button is pressed
		while(! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1));

		//Wait to avoid button de-bouncing related issues
		delay();

		//see what is the length of the data received from the slave
		commandcode = 0x51; //Command

		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//Receive the length of the data received from the slave
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//Receive data from the slave
		commandcode = 0x52; //Receive data command

		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		//Receive data
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		rxComplt = RESET;

		//Wait till RX completes
		while(rxComplt != SET);
		rcv_buf[len+1]= '\0';
		//printf("Data : %s", rcv_buf);
		rxComplt = RESET;
	}

}

void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	if(AppEv == I2C_EV_TX_CMPLT)
	{
		printf("TX is completed\n");
	}else if(AppEv == I2C_EV_RX_CMPLT)
	{
		printf("RX is completed\n");
		rxComplt = SET;

	}else if (AppEv == I2C_ERROR_AF)
	{
		printf("Error: Ack failure\n");
		//in master ack failure happens when slave fails to send ack for the byte sent from the master.
		I2C_CloseSendData(pI2CHandle);

		//generate the stop condition to release the bus
		I2C_GenerateStopCondition(I2C1);

		//Hang in infinite loop
		while(1);
	}
}
