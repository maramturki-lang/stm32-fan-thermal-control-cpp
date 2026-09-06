#include "STM_Gpiogroupe.hpp"
#include "stm32f4xx_hal.h"


void STM_gpiogr::init(uint16_t Pin, uint32_t Mode,
                      uint32_t Pull, uint32_t Speed,
                      uint32_t Alternate)
{
    ownedPins |= Pin;

    GPIO_InitTypeDef GP = {0};

    GP.Pin = Pin;
    GP.Mode = Mode;
    GP.Pull = Pull;
    GP.Speed = Speed;

    if (Mode == GPIO_MODE_AF_PP || Mode == GPIO_MODE_AF_OD)
    {
        GP.Alternate = Alternate;
    }

    HAL_GPIO_Init(_port, &GP);
}


//s'il y'a une erreur le programme va s'arreter directement
//et quand je regarde le call stack je peux connaitre quelle fonction(write,read,toggle,...) a appeler l'erreur handler

void STM_gpiogr::write(uint16_t pins, GPIO_PinState state)
{
    uint16_t validPins = pins & ownedPins;

    if (validPins == 0)
    {
        Error_Handler();
    }

    HAL_GPIO_WritePin(_port, validPins, state); // state = GPIO_PIN_RESET = 0 logique ou GPIO_PIN_SET   = 1 logique
}


GPIO_PinState STM_gpiogr::read(uint16_t pins)
{
    uint16_t validPins = pins & ownedPins;

    if (validPins == 0)
    {
        Error_Handler();
    }

    return HAL_GPIO_ReadPin(_port, validPins);      // HAL_GPIO_ReadPin(); retourne l'etat de la pin lise 0 ou 1 (GPIO_PinState)
}


void STM_gpiogr::toggle(uint16_t pins)
{
    uint16_t validPins = pins & ownedPins;

    if (validPins == 0)
    {
        Error_Handler();   // erreur runtime volontaire
    }

    HAL_GPIO_TogglePin(_port, validPins);
}
