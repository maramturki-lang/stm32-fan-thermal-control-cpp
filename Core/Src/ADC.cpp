#include "ADC.hpp"

void STM_ADC::init() {
    _hadc = ADC_HandleTypeDef{};
    _hadc.Instance = _adc;

    // Configuration de l'ADC
    _hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
    _hadc.Init.Resolution            = ADC_RESOLUTION_12B;
    _hadc.Init.ScanConvMode          = ENABLE;              // on veut convertir plusieurs canaux dans une séquence, Rank1:Temperature,Rank2:Potentiometre
    _hadc.Init.ContinuousConvMode    = DISABLE;             // Pas de boucle infinie
    _hadc.Init.DiscontinuousConvMode = ENABLE;              // On découpe le groupe
    _hadc.Init.NbrOfDiscConversion   = 1;                   // 1 conversion par déclenchement,à declanchement, l’ADC convertit un seul canal de la séquence,1er start --> Rank1,2eme start --> Rank2,3eme start --> Rank1,...
    _hadc.Init.NbrOfConversion       = 2;                   // Taille totale du groupe
    _hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    _hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    _hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    _hadc.Init.DMAContinuousRequests = DISABLE;
    _hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV; // IT après chaque canal

    //HAL_ADC_Init(&_hadc);

    if (HAL_ADC_Init(&_hadc) != HAL_OK)
    {
        Error_Handler();
    }

    // Configuration des Rangs du groupe
    ADC_ChannelConfTypeDef sConfig = {0}; // Structure de configuration d'un canal ADC et contient : Channel, Rank,SamplingTime
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

    // Rang 1
    sConfig.Channel = _channels[0];
    sConfig.Rank = 1;                          // Channel 1 va etre configurer en premier

    if (HAL_ADC_ConfigChannel(&_hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    // On remplie l'objet de la structure sConfig deux fois avec des valeurs differentes

    // Rang 2
    sConfig.Channel = _channels[1];
    sConfig.Rank = 2;

    if (HAL_ADC_ConfigChannel(&_hadc, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


void STM_ADC::startNextConversionIT() {
    HAL_ADC_Start_IT(&_hadc);            //Lance une conversion ADC et active l’interruption de fin de conversion.
}

uint32_t STM_ADC::getValue() {
    return HAL_ADC_GetValue(&_hadc);
}
