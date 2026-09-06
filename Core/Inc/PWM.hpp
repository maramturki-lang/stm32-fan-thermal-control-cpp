#ifndef PWM_HPP
#define PWM_HPP

#include "Timer.hpp"

class STM_PWM : public STM_BASETIMER
{
private:

public:
    // Constructeur
    STM_PWM(TIM_TypeDef* timer); //uint32_t channel);

    // Initialisation PWM
    void PWM_Init(uint32_t Period,
                 uint32_t Prescaler);

    void PWM_Config_Channel (uint32_t Channel,uint32_t Pulse);

    // Commande du rapport cyclique
    void SET_DUTY(uint32_t Channel, uint32_t Pulse);

    // Démarrage / arrêt PWM
    void PWM_START_CHANNEL(uint32_t Channel);
    void PWM_STOP_CHANNEL(uint32_t Channel);
};

#endif
