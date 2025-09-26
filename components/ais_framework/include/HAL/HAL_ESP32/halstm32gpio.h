/*
 * halstm32gpio.h
 *
 *  Created on: May 13, 2024
 *      Author: canhpn2
 */

#ifndef HAL_HAL_STM32_INC_HALSTM32GPIO_H_
#define HAL_HAL_STM32_INC_HALSTM32GPIO_H_

#include "halgpioabstract.h"

class HalEsp32Gpio:public HalGpioAbstract
{
public:
	HalEsp32Gpio();
	virtual ~HalEsp32Gpio();

	bool Hal_Gpio_ReadPin(uint16_t pin) override;
	void Hal_Gpio_WritePin(uint16_t pin, GpioPinState state) override;
	void Hal_Gpio_TogglePin(uint16_t pin) override;
};

#endif /* HAL_HAL_STM32_INC_HALSTM32GPIO_H_ */
