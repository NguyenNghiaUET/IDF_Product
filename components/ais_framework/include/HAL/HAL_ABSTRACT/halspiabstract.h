/*
 * halstm32spiabstract.h
 *
 *  Created on: May 13, 2024
 *      Author: canhpn2
 */

#ifndef HAL_STM32_ABSTRACT_INC_HALSTM32SPIABSTRACT_H_
#define HAL_STM32_ABSTRACT_INC_HALSTM32SPIABSTRACT_H_
#include  <stdint.h>

class HalSpiAbstract
{
public:
	virtual bool Hal_Spi_Transmit(uint8_t* data, uint16_t size, uint32_t timeout) = 0;
	virtual bool Hal_Spi_Receive(uint8_t* data, uint16_t size, uint32_t timeout) = 0;
	virtual bool Hal_Spi_TransmitReceive(uint8_t* pTxData,uint8_t* pRxData, uint16_t Size, uint16_t Timeout) = 0;
};

#endif /* HAL_STM32_ABSTRACT_INC_HALSTM32SPIABSTRACT_H_ */
