/*
 * stm32f401xx_spi_driver.h
 *
 *  Created on: Apr 11, 2025
 *      Author: phucv
 */

#ifndef INC_STM32F401XX_SPI_DRIVER_H_
#define INC_STM32F401XX_SPI_DRIVER_H_

#include "stm32f401xx.h"
/*
 * Configuration structure for SPIx peripheral
 */
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;

/*
 * This is a handle structure for a SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t 	*pSPIx; 			/*This holds the base address of the SPI */
	SPI_Config_t	SPIConfig;			/*This holds SPI configuration settings */
	uint8_t			*pTxBuffer;			/*To store the application TX buffer address*/
	uint8_t 		*pRxBuffer;			/*To store the application RX buffer address*/
	uint32_t		TxLen;				/*To store Tx len*/
	uint32_t		RxLen;				/*To store Rx len*/
	uint8_t			TxState;			/*To store Rx state*/
	uint8_t			RxState;			/*To store Rx state*/
}SPI_Handle_t;


/*
 * @SPI_DeviveMode
 */
#define SPI_DEVICE_MODE_MASTER		1
#define SPI_DEVICE_MODE_SLAVE		0

/*
 * @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD					1	//Full-duplex
#define SPI_BUS_CONFIG_HD					2	//Half-duplex
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY		3	//Simplex-RX-Only

/*
 * @SPI_SclkSpeed
 */
#define SPI_SCLK_SPEED_DIV2			0
#define SPI_SCLK_SPEED_DIV4			1
#define SPI_SCLK_SPEED_DIV8			2
#define SPI_SCLK_SPEED_DIV16		3
#define SPI_SCLK_SPEED_DIV32		4
#define SPI_SCLK_SPEED_DIV64		5
#define SPI_SCLK_SPEED_DIV128		6
#define SPI_SCLK_SPEED_DIV256		7

/*
 * @SPI_DFF
 */
#define SPI_DFF_8BITS		0
#define SPI_DFF_16BITS		1

/*
 * @SPI_CPOL
 */
#define SPI_CPOL_HIGH	1
#define SPI_CPOL_LOW	0

/*
 * @SPI_CPHA
 */
#define SPI_CPHA_HIGH	1
#define SPI_CPHA_LOW	0

/*
 * @SPI_CPHA
 */
#define SPI_SSM_DI		0
#define SPI_SSM_EN		1

/*
 * SPI related status flags definitions
 */
#define SPI_TXE_FLAG	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG	(1 << SPI_SR_BSY)


/*
 * Posible SPI Application events
 */
#define SPI_EVENT_TX_CMPLT 	1	//Transmission complete
#define SPI_EVENT_RX_CMPLT 	2	//Reception complete
#define SPI_EVENT_OVR_ERR 	3	//Overrun error
#define SPI_EVENT_CRC_ERR 	4	//CRC error


/*
 * Possible SPI Application States
 */
#define SPI_READY		0
#define SPI_BUSY_IN_RX	1
#define SPI_BUSY_IN_TX	2

/*********************************************************************
 * 							APIs
 * ********************************************************************/
/*
 * Peripheral clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void SPI_Init(SPI_Handle_t* pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t* pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t* pRxBuffer, uint32_t Len);

/*
 * Data Send and Receive with Interrupt
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t* pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t* pRxBuffer, uint32_t Len);


/*
 * IRQ Configuration and IRQ Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t* pSPIHandle);

/*
 * Other Peripheral Control APIs
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

/*
 * Application callback
 */
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);



#endif /* INC_STM32F401XX_SPI_DRIVER_H_ */
