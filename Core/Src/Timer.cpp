#include "Timer.hpp"


void STM_BASETIMER::BaseInit(uint32_t period,
							 uint32_t pscal ,
							 uint32_t countmode ){

	  _htim.Instance = _Timer;
	  _htim.Init.Prescaler = pscal;      	  //Une autre structure TIM_Base_InitTypeDef Init; dans la structure ( ou le handle) TIM_HandleTypeDef _htim;
	  _htim.Init.CounterMode = countmode;
	  _htim.Init.Period = period;
	  _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  _htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	  // HAL_TIM_Base_Init(&_htim);


//Les fonctions HAL retournent un type appelé :
//	HAL_StatusTypeDef

//	Ce type peut avoir plusieurs valeurs :

//	HAL_OK       // tout est bon
//	HAL_ERROR    // erreur
//	HAL_BUSY     // périphérique occupé
//	HAL_TIMEOUT  // temps dépassé

	  if (HAL_TIM_Base_Init(&_htim) != HAL_OK)
	  {
	      Error_Handler();
	  }

}

// Méthodes de manipulation
void STM_BASETIMER::BaseStart(void){
	__HAL_TIM_ENABLE(&_htim);
	//HAL_TIM_Base_Start(&_htim);      // Les deux démarrent le timer, mais cette fonction HAL est plus propre.
}
void STM_BASETIMER::BaseStop(void){
	__HAL_TIM_DISABLE(&_htim);
}
void STM_BASETIMER::BaseStart_IT(void) {
    __HAL_TIM_ENABLE_IT(&_htim,TIM_IT_UPDATE); // Cette ligne active seulement l’interruption update.
    //HAL_TIM_Base_Start_IT(&_htim);          //  Celle la demarre le timer et active l'interruption update.
}

void STM_BASETIMER::BaseStop_IT(void) {
	__HAL_TIM_DISABLE_IT(&_htim,TIM_IT_UPDATE);
}

