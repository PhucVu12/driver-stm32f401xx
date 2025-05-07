/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: Apr 20, 2025
 *      Author: phucv
 */

#include <stdio.h>
#include "stm32f401xx.h"
#include <string.h>

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


	//Button init
	GPIO_ButtonInits();

	//Configure the I2C1 GPIO pin
	I2C1_GPIOInits();

	//I2C1 peripheral configuration
	I2C1_Inits();

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
		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
		//Receive the length of the data received from the slave
		I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		//Receive data from the slave
		commandcode = 0x52; //Receive data command
		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
		//Receive data
		I2C_MasterReceiveData(&I2C1Handle, rcv_buf, len, SLAVE_ADDR, I2C_DISABLE_SR);

		rcv_buf[len+1]= '\0';
		//printf("Data : %s", rcv_buf);

	}

}

