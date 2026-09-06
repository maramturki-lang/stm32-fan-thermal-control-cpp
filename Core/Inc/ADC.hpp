#ifndef ADC_HPP
#define ADC_HPP


#include "stm32f4xx_hal.h"
#include "main.hpp"


class STM_ADC {
private:
	ADC_TypeDef* _adc;
    ADC_HandleTypeDef _hadc= {0};
    uint32_t _channels[2];

public:
    // Constructeur : on définit deux chaînes physiques
    STM_ADC(ADC_TypeDef* adc, uint32_t ch1, uint32_t ch2)
        : _adc(adc), _hadc{}
    {
        _hadc.Instance = _adc;
        _channels[0] = ch1;
        _channels[1] = ch2;
    }

    // Initialise l'ADC en mode SCAN + DISCONTINU
    void init();

    // Lance UNE conversion (la suivante dans le groupe)
    void startNextConversionIT();

    // Lit la valeur convertie (à appeler dans l'interruption)
    uint32_t getValue();

    // Accesseur pour le handle (nécessaire pour le lien avec HAL)
    ADC_HandleTypeDef* getHandle() { return &_hadc; }
};


#endif
