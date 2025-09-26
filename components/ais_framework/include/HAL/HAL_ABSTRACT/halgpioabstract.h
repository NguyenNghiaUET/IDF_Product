/*
 * halstm32gpioabstract.h
 *
 *  Created on: May 13, 2024
 *      Author: canhpn2
 */

#ifndef HAL_STM32_INC_HALSTM32GPIOABSTRACT_H_
#define HAL_STM32_INC_HALSTM32GPIOABSTRACT_H_
#include  <stdint.h>

class HalGpioAbstract
{
public:
	enum class GpioPinState : uint8_t
	{
		GPIO_PIN_RESET = 0, GPIO_PIN_SET
	};

	virtual bool Hal_Gpio_ReadPin(uint16_t pin) = 0;
	virtual void Hal_Gpio_WritePin(uint16_t pin, GpioPinState state) = 0;
	virtual void Hal_Gpio_TogglePin(uint16_t pin) = 0;
};

#endif /* HAL_STM32_INC_HALSTM32GPIOABSTRACT_H_ */
