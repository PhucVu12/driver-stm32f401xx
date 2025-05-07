/*
 * stm32f401xx_spi_driver.c
 *
 *  Created on: Apr 11, 2025
 *      Author: phucv
 */

#include "stm32f401xx_spi_driver.h"

//prototype for private function
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);
/*
 * Peripheral clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
	}else{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}


/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t* pSPIHandle)
{
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);
	//Configure the SPI_CR1 register
	uint32_t tempreg = 0;
	// 1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	//2. Configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD )
	{
		//BIDI mode should be clear
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE );
	}else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//BIDI mode should be set
		tempreg |= ( 1 << SPI_CR1_BIDIMODE );
	}else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//BIDI mode should be clear
		tempreg &= ~( 1 << SPI_CR1_BIDIMODE );
		//RXONLY bit must be set
		tempreg |= ( 1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the spi serial clock speed (baud rate)
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;
	// 4. Configure the DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;
	// 5. Configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;
	// 6. Configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;
	// 7. Configure the SSM
	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	// use tempreg as a regigter 32 bits
	pSPIHandle->pSPIx->CR1 = tempreg;
}

void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}
/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t* pTxBuffer, uint32_t Len)
{
	//This is blocking API

	while( Len > 0 )
	{
		// 1. Wail until TXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);
		//while ( ! ( pSPIx->SR & (1 << 1 ) ) );

		// 2. Check the DFF bit in CR1
		if( (pSPIx->CR1 & ( 1 << SPI_CR1_DFF )) )
		{
			// 16 bits DFF
			// 1.Load the data into the DR
			pSPIx->DR = *((uint16_t*) pTxBuffer);
			Len--;
			Len--;
			(uint16_t*) pTxBuffer++;
		}else
		{
			//8 bits DFF
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}

	}
}
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t* pRxBuffer, uint32_t Len)
{

	while( Len > 0 )
	{
		// 1. Wail until RXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// 2. Check the DFF bit in CR1
		if( (pSPIx->CR1 & ( 1 << SPI_CR1_DFF )) )
		{
			// 16 bits DFF
			// 1.Load the data from DR to Rx buffer
			 *((uint16_t*) pRxBuffer) = pSPIx->DR ;
			Len--;
			Len--;
			(uint16_t*) pRxBuffer++;
		}else
		{
			//8 bits DFF
			 *pRxBuffer = pSPIx->DR ;
			Len--;
			pRxBuffer++;
		}
	}
}

/*
 * SPI_PeripheralControl
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= ( 1 << SPI_CR1_SPE );
	}else if(EnorDi == DISABLE)
	{
		pSPIx->CR1 &= ~( 1 << SPI_CR1_SPE );
	}
}

/*
 * SPI_SSI configuration
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= ( 1 << SPI_CR1_SSI );
	}else if(EnorDi == DISABLE)
	{
		pSPIx->CR1 &= ~( 1 << SPI_CR1_SSI );
	}
}

/*
 * SPI_SSOE configuration
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR2 |= ( 1 << SPI_CR2_SSOE );
	}else if(EnorDi == DISABLE)
	{
		pSPIx->CR2 &= ~( 1 << SPI_CR2_SSOE );
	}
}


/*
 * Data Send and Receive with Interrupt
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t* pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX)
	{
		//1. Save the Tx buffer address and len information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;
		//2. Mark the SPI state as busy in transmission so that
		//no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;
		//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_TXEIE);

	}
	return state;
	//4. Data Transmission will be handled by the IRQ code
}
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t* pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;
	if(state != SPI_BUSY_IN_RX)
	{
		//1. Save the Rx buffer address and len infomation in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		//2. Mark the SPI state as busy in receive so that
		//no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;
		//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= ( 1 << SPI_CR2_RXNEIE);

	}
	return state;
	//4. Data Transmission will be handled by the IRQ code
}

/*
 * IRQ Configuration and IRQ Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ISER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )// 32 to 63
		{
			*NVIC_ISER1 |= (1 << (IRQNumber % 32 ));
		}else if(IRQNumber >= 64 && IRQNumber < 96)// 64 to 95
		{
			*NVIC_ISER2 |= (1 << (IRQNumber % 64 ));
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )// 32 to 63
		{
			*NVIC_ICER1 |= (1 << (IRQNumber % 32 ));
		}else if(IRQNumber >= 64 && IRQNumber < 96)// 64 to 95
		{
			*NVIC_ICER2 |= (1 << (IRQNumber % 64 ));
		}
	}
}
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;
	uint8_t shift_amount =  (8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASE_ADDR + iprx ) |= ( IRQPriority << shift_amount );
}

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// Check the DFF bit in CR1
	if( (pSPIHandle->pSPIx->CR1 & ( 1 << SPI_CR1_DFF )) )
	{
		// 16 bits DFF
		// 1.Load the data into the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*) pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;
		(uint16_t*) pSPIHandle->pTxBuffer++;
	}else
	{
		//8 bits DFF
		pSPIHandle->pSPIx->DR = *(pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if( ! pSPIHandle->TxLen)
	{
		//TXLen is zero, so close SPI transmission and inform the application that
		//TX is over.
		//This prevents interrupts from setting up of TXE flag
		SPI_CloseTransmission(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_TX_CMPLT);

	}
}
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// 2. Check the DFF bit in CR1
	if( (pSPIHandle->pSPIx->CR1 & ( 1 << SPI_CR1_DFF )) )
	{
		// 16 bits DFF
		// 1.Load the data from DR to Rx buffer
		 *((uint16_t*) pSPIHandle->pRxBuffer) = (uint16_t) pSPIHandle->pSPIx->DR ;
		 pSPIHandle->RxLen--;
		 pSPIHandle->RxLen--;
		 pSPIHandle->pRxBuffer--;
		 pSPIHandle->pRxBuffer--;
	}else
	{
		//8 bits DFF
		 *(pSPIHandle->pRxBuffer) = (uint8_t) pSPIHandle->pSPIx->DR ;
		 pSPIHandle->RxLen--;
		 pSPIHandle->pRxBuffer--;
	}
	if( ! pSPIHandle->RxLen)
		{
			//RXLen is zero, so close SPI transmission and inform the application that
			//RX is over.
			//This prevents interrupts from setting up of RXE flag
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_RX_CMPLT);

		}

}
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	//1. clear the ovr flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void) temp;
	//2. inform the application
	SPI_ApplicationEventCallback(pSPIHandle,SPI_EVENT_OVR_ERR);
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}

void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void) temp;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	//This is a weak implementation, the user application may override this function
}


void SPI_IRQHandling(SPI_Handle_t* pSPIHandle)
{
	uint8_t temp1, temp2;
	//first check for TXE
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_TXE );
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_TXEIE);
	if (temp1 && temp2) //TX buffer empty and Tx buffer empty interrupt enable
	{
		//handle TXE
		spi_txe_interrupt_handle(pSPIHandle);

	}

	//check for RXNE
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE );
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_RXNEIE);
	if (temp1 && temp2) //RX buffer empty and Rx buffer empty interrupt enable
	{
		//handle TXE
		spi_rxne_interrupt_handle(pSPIHandle);

	}

	//check for Overrun flag(OVR)
	temp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_OVR );
	temp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_ERRIE);
	if (temp1 && temp2) //RX buffer empty and Rx buffer empty interrupt enable
	{
		//handle TXE
		spi_ovr_err_interrupt_handle(pSPIHandle);

	}


}


