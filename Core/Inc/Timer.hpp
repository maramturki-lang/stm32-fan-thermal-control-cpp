#ifndef TIMER_HPP
#define TIMER_HPP

#include "stm32f4xx_hal.h"
#include "main.hpp"



class STM_BASETIMER{

protected:
	TIM_TypeDef* _Timer;
	TIM_HandleTypeDef _htim;  // On stocke le handle ici qui est une structure contenante tout les parametre de notre
	                         // Timer, utilisee pour tout les peripheriques sauf pour les GPIO

public:
    // Constructeur demandé
	STM_BASETIMER(TIM_TypeDef* timer) : _Timer(timer), _htim{} {}

	// Méthode de configuration groupée
	    void BaseInit(uint32_t period, uint32_t pscal = 0,
	                uint32_t countmode = TIM_COUNTERMODE_UP);

	    // Méthodes de manipulation
	    void BaseStart(void);

	    void BaseStop(void);
	    // Contrôle avec Interruptions (IT)
	        void BaseStart_IT();

	        void BaseStop_IT();

	    TIM_HandleTypeDef* getHandle() { return &_htim; }

};

#endif

