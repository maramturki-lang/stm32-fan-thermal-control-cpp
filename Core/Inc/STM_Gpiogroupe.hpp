#ifndef STM_GPIOGROUPE_HPP
#define STM_GPIOGROUPE_HPP
#include "cstdint"
#include "stm32f4xx_hal.h"
#include "main.hpp"


class STM_gpiogr
{
private :
	GPIO_TypeDef *_port; // GPIOA, GPIOB ....
	uint16_t ownedPins;

public :
	// Constructeur
	STM_gpiogr(GPIO_TypeDef *port) : _port(port), ownedPins(0) {}

	void init (uint16_t Pin,
			    uint32_t Mode,
				  uint32_t Pull = GPIO_NOPULL,
			        uint32_t Speed = GPIO_SPEED_FREQ_MEDIUM,
					 uint32_t Alternate = 0);

	void write (uint16_t pins, GPIO_PinState state);

	GPIO_PinState read (uint16_t pins);

	void toggle (uint16_t pins);


};

#endif
