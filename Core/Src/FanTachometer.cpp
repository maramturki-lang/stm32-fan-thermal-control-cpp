#include "FanTachometer.hpp"

STM_FanTachometer::STM_FanTachometer(TIM_TypeDef* timer, uint32_t channel)
    : STM_BASETIMER(timer), _channel(channel)
{
}

void STM_FanTachometer::init(uint32_t period, uint32_t prescaler)
{
    BaseInit(period, prescaler, TIM_COUNTERMODE_UP);

    if (HAL_TIM_IC_Init(getHandle()) != HAL_OK)
    {
        Error_Handler();
    }

    TIM_IC_InitTypeDef sConfigIC = {0};

    sConfigIC.ICPolarity  = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter    = 0;

    HAL_TIM_IC_ConfigChannel(getHandle(), &sConfigIC, _channel);
}

void STM_FanTachometer::startIT()
{
	HAL_TIM_IC_Start_IT(getHandle(), _channel);
}

uint32_t STM_FanTachometer::readCapture()
{
    return HAL_TIM_ReadCapturedValue(getHandle(), _channel);
}

void STM_FanTachometer::resetCounter()
{
    __HAL_TIM_SET_COUNTER(getHandle(), 0);
}
