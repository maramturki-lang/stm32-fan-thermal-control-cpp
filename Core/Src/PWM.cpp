#include "PWM.hpp"

STM_PWM::STM_PWM(TIM_TypeDef* timer): STM_BASETIMER(timer)
{
}

void STM_PWM::PWM_Init(uint32_t Period,
                      uint32_t Prescaler)
{
    // Initialisation du timer de base (héritée)
    BaseInit(Period, Prescaler);

    //HAL_TIM_PWM_Init(&_htim);

    if (HAL_TIM_PWM_Init(getHandle()) != HAL_OK)
    {
        Error_Handler();
    }

}
                                                        // Duty Cycle
void STM_PWM::PWM_Config_Channel (uint32_t Channel,uint32_t Pulse){

	TIM_OC_InitTypeDef sConfig = {0};

	sConfig.OCMode = TIM_OCMODE_PWM1;
	sConfig.Pulse = Pulse;
	sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode = TIM_OCFAST_DISABLE;

	HAL_TIM_PWM_ConfigChannel(getHandle(), &sConfig, Channel);

}

void STM_PWM::SET_DUTY(uint32_t Channel, uint32_t pulse)
{
    __HAL_TIM_SET_COMPARE(getHandle(), Channel, pulse);
}

void STM_PWM::PWM_START_CHANNEL(uint32_t Channel)
{
    HAL_TIM_PWM_Start(getHandle(), Channel);
}

void STM_PWM::PWM_STOP_CHANNEL(uint32_t Channel)
{
    HAL_TIM_PWM_Stop(getHandle(), Channel);
}
