/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <main.hpp>
#include "STM_Gpiogroupe.hpp"
#include "Timer.hpp"
#include "PWM.hpp"
#include "UART.hpp"
#include "ADC.hpp"
#include "FanTachometer.hpp"
#include <sstream>


//Globales car on va les utiliser en dohrs de la main

STM_gpiogr Bouton(GPIOA);     // PA0 (USER Button)
STM_gpiogr PWM_Output(GPIOA); // PA6 (TIM3_CH1)
STM_gpiogr UART_Pins(GPIOA);  // PA2(TX) et PA3(RX)
STM_gpiogr ADC_Pins (GPIOA);  // PA1 (ADC1_CH1) PA4 (ADC1_CH4)
STM_gpiogr Tach_Pin(GPIOA);   // PA5 = TIM2_CH1



STM_PWM PWM_Fan(TIM3);
STM_UART UART2(USART2);
STM_ADC Adc1(ADC1,ADC_CHANNEL_1,ADC_CHANNEL_4);
STM_FanTachometer myFan(TIM2, TIM_CHANNEL_1);


volatile float TempC = 0 ;   // Valeur Temperature
volatile float Consigne = 0; // Valeur Potentiometre
volatile bool sem = false ; // semaphore semaha prof
volatile bool modeCelsius = true;


volatile uint32_t tachCapture = 0;
volatile bool tachReady = false;
uint32_t RPM = 0;
uint32_t PWM_Image = 0;
uint32_t lastTachTick = 0;

uint32_t TIMER_IC_FREQ = 1000000;      // TIM2 compte à 1 MHz donc 1 tick = 1 us
uint32_t TACH_PULSES_PER_REV = 2;      // Le ventilateur donne généralement 2 impulsions par tour
uint32_t RPM_MAX = 2500;               // Vitesse max approximative du ventilateur

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Configure the system clock */
  SystemClock_Config();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_ADC1_CLK_ENABLE();



  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */


  Bouton.init(GPIO_PIN_0, GPIO_MODE_IT_RISING);
  HAL_NVIC_SetPriority(EXTI0_IRQn,0,0);       // NVIC C’est le bloc matériel dans le Cortex-M4 qui : gère toutes les interruptions, décide laquelle exécuter en priorité et permet d’activer/désactiver les IRQ
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);



  /*On fixe une période PWM (ARR), puis on calcule une valeur de CCR en fonction de la température,
	  et on l’applique pour contrôler la vitesse du ventilateur. */

  PWM_Output.init(GPIO_PIN_6, GPIO_MODE_AF_PP, GPIO_NOPULL,
                   GPIO_SPEED_FREQ_HIGH,  /* Alternate = */ GPIO_AF2_TIM3);
  PWM_Fan.PWM_Init(1000, 83);                         // ARR=1000, PSC=83
  PWM_Fan.PWM_Config_Channel(TIM_CHANNEL_1, 0);    // CCR = 500, duty 500/1000 = 50%    ,CNT < CCR → ON , CNT >= CCR → OFF
  PWM_Fan.PWM_START_CHANNEL(TIM_CHANNEL_1);



  //               TX           RX(Pas besoin)
  UART_Pins.init(GPIO_PIN_2 | GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_NOPULL,
		           GPIO_SPEED_FREQ_HIGH,GPIO_AF7_USART2);
  UART2.init(115200);





  ADC_Pins.init(GPIO_PIN_1 | GPIO_PIN_4 , GPIO_MODE_ANALOG);
  Adc1.init();
  HAL_NVIC_SetPriority(ADC_IRQn,0,0);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
  Adc1.startNextConversionIT();



  Tach_Pin.init(GPIO_PIN_5,GPIO_MODE_AF_PP,
                GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH,GPIO_AF1_TIM2);
  /*Pourquoi on met ARR très grand ?
  Parce qu’en Input Capture, on veut mesurer le temps entre deux fronts montants du signal Tach.
  Si ARR est trop petit, le compteur risque de déborder avant le prochain front.*/
  myFan.init(0xFFFFFFFF, 15);            //0xFFFFFFFF est la valeur maximale sur 32 bits.comme notre compteur est de 32bits
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  myFan.startIT();


  /*PSC = 15
  → divise 16 MHz par 16
  → compteur à 1 MHz
  → 1 tick = 1 µs // pas de comptage
  ARR = 0xFFFFFFFF = 4 294 967 295
  → valeur maximale du compteur TIM2
  → évite le débordement
  → permet de mesurer des périodes longues

  4 294 967 295 µs ≈ 4294 s ≈ 71 minutes*/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  UART2.send("Beginning : \r\n");

  while (1)
  {
    /* USER CODE END WHILE */

	  //Comme ton TIM2 compte à 1 MHz : 1 tick = 1 µs
	  //Donc si :capture = 16000;
	  //cela veut dire : période Tach = 16000 µs = 16 ms

	  HAL_Delay(1000);

	  if (tachReady)            //Dans TIM2_IRQHandler, après une capture valide :on met tachReady a true,Il sert à dire au main : Une nouvelle période Tach est disponible.
	  {
	      uint32_t capture = tachCapture;  // Periode Tach mesuree en ticks et copier depuis la variable globale tachCapture
	      tachReady = false;

	      if (capture != 0)
	      {
	          RPM = (60UL * TIMER_IC_FREQ) / (TACH_PULSES_PER_REV * capture);

	          PWM_Image = (RPM * 100UL) / RPM_MAX;

	          if (PWM_Image > 100)
	          {
	              PWM_Image = 100;
	          }
	      }
	  }


	    if (sem)                         // sem synchronise la conversion avec la boucle principale tout simplement
	    {                                 // c’est a dire que rien ne c’est envoyé que si les deux conversions se termine.

	    	static int CCR = 0;
	    	float erreurTemp = TempC - Consigne;
	    	uint16_t PWM_exige = 0;

	    	if (erreurTemp <= 0)
	    	{
	    	    PWM_exige = 0;
	    	    CCR = 0;  // ventilateur OFF
	    	}
	    	else
	    	{
	    	    float Kp_temp = 10.0f;
	    	    PWM_exige = 420 + (uint16_t)(Kp_temp * erreurTemp);

	    	    if (PWM_exige > 1000)
	    	        PWM_exige = 1000;

	    	    float erreurVitesse = (float)PWM_exige - ((float)PWM_Image * 10.0f);

	    	    float Kp_vitesse = 0.2f;
	    	    CCR = CCR + (int)(Kp_vitesse * erreurVitesse);

	    	    if (CCR < 420)
	    	        CCR = 420;   // vitesse minimale du ventilateur

	    	    if (CCR > 1000)
	    	        CCR = 1000;
	    	}

	        PWM_Fan.SET_DUTY(TIM_CHANNEL_1, CCR);

	        std::ostringstream oss;


	        if (modeCelsius)
	        {
	            int Temp10 = (int)(TempC * 10.0f);
	            int Cons10 = (int)(Consigne * 10.0f);

	            oss << "Temperature = " << (Temp10 / 10) << "." << (Temp10 % 10) << "C" << "\r\n";  // on cree un string propre avec cette methode avant de l'envoyer sur UART
	            oss << "Consigne = " << (Cons10 / 10) << "." << (Cons10 % 10) << "C" << "\r\n";
	        }
	        else
	        {
	            int TempK10 = (int)((TempC + 273.15f) * 10.0f);
	            int ConsK10 = (int)((Consigne + 273.15f) * 10.0f);

	            oss << "Temperature = " << (TempK10 / 10) << "." << (TempK10 % 10) << "K" << "\r\n";
	            oss << "Consigne = " << (ConsK10 / 10) << "." << (ConsK10 % 10) << "K" << "\r\n";
	        }

	        oss << "Duty Cycle = " << CCR / 10 << "%" << "\r\n";
	        oss << "Tach Period = " << tachCapture << " us" << "\r\n";
	        oss << "RPM = " << RPM << " tr/min" << "\r\n";
	        oss << "PWM Image = " << PWM_Image << "%" << "\r\n\r\n";

	        /*
	        char msg[50];
	        sprintf(msg, "Temp = %lu\r\n", temp);    // on cree un string propre avec une autre methode
	        UART2.send((uint8_t*)msg, strlen(msg));  // Avec la deuxieme fonction send
	        */

	        UART2.send(oss.str()); // oss cree le message, str recupere ce message dans send

	        sem = false;
	        Adc1.startNextConversionIT(); // Relance une nouvelle conversion
	    }


    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/*
 Il faut 3 conditions pour accepter une interruption :
 - L’événement arrive
 - L’interruption est autorisée
 - NVIC activé
  */


/*
 Le périphérique GPIO possède un registre qui contient 16 flags chaque flags est dedié à une line ,
 par exemple EXTI0, EXTI1,… et chacune de ces lignes est reliée a une pin avec le même numéros.

 EXIT0 peut être reliée à PA0,PB0,… Mais seulement un a la fois.

 Et si je relie une pins sur une ligne d’interruption chaque signal qui viens sur cette ligne
 avec le meme nature de ce que j’ai definis (front montant ou descendant )
 va mettre le flag correspondant à 1 et déclencher le handler si je veux. */




/*
 TIM_FLAG_UPDATE si = 1 donc Le timer a généré un événement d’update (overflow/underflow).

 Chaque timer possède un registre de flags mais la majorité de ces flags sont inutile pour
 nous mais le seule important est TIM_FLAG_UPDATE.

 TIM_IT_UPDATE n’est considéré un flag il est un bit d’activation ou désactivation d’interruption
 si je le met à 1 alors le timer peut générer une interruption LORSQU’un événement UPDATE(overflow) se produit.
 */

extern "C" {

void EXTI0_IRQHandler(void){

    if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_0)){ // on lit le flag correspondant aux GPIO_PIN_0 ou (EXIT0)

    	// Ici on va changer l'affichage de la temperature : C/K
    	modeCelsius = !modeCelsius;

        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);

    }
  }



/*Ce handler est exécuté quand TIM2 détecte un front montant sur le signal Tach du ventilateur.*/
void TIM2_IRQHandler(void)
{
    static bool firstCapture = true;

    if (__HAL_TIM_GET_FLAG(myFan.getHandle(), TIM_FLAG_CC1) != RESET) // Capture/Compare Channel 1 flag,Ce flag passe à 1 quand une capture a eu lieu sur le canal 1. Donc cette condition veut dire : Est-ce que TIM2_CH1 a capturé une valeur ? Autrement dit : Est-ce qu’un front montant Tach a été détecté ? Donc n verifie si l'evenemnet c'est produit sur le channel 1 du TIM2
    {
        if (__HAL_TIM_GET_IT_SOURCE(myFan.getHandle(), TIM_IT_CC1) != RESET) // Cette ligne vérifie que l’interruption du canal 1 est bien activée.
        {
            __HAL_TIM_CLEAR_IT(myFan.getHandle(), TIM_IT_CC1);  // On efface le flag d'interruption pour leprocesseur ne pense pas quelle est toujours presente

            if (firstCapture)            //Donc au début : firstCapture = true et Après le premier front : firstCapture = false et elle reste false.
            {
                myFan.resetCounter();
                firstCapture = false;
            }
            else
            {
                tachCapture = myFan.readCapture(); // valeur CCR = période Tach en ticks

                myFan.resetCounter();              // reset CNT pour mesurer la prochaine période

                tachReady = true;                  // nouvelle mesure disponible pour le main
            }
        }
    }
}


void ADC_IRQHandler(void)
{
    static bool cond = true;

    if (__HAL_ADC_GET_FLAG(Adc1.getHandle(), ADC_FLAG_EOC)) // ADC_FLAG_EOC indique que la conversion ADC en cours est terminée.
                                                           // Ici, comme on convertit les canaux un par un, ce flag se déclenche à chaque canal converti.
    {                                                      // (On a generalement un seule ADC par carte).

        if (cond)
        {
            uint32_t adc_value = Adc1.getValue();

            float Vadc = adc_value * 3.3f / 4095.0f;       // 4095 --> 3,3V
            TempC = Vadc / 0.01f;                         // le capteur température type LM35 : 10 mV / °C = 0.01 V / °C, donne 0.01V/C

            //TempC = (adc_value * 330.0f) / 4095.0f;

            cond = false;

            __HAL_ADC_CLEAR_FLAG(Adc1.getHandle(), ADC_FLAG_EOC);

            Adc1.startNextConversionIT(); // On lance la conversion suivante : la consigne
        }
        else
        {
            uint32_t pot = Adc1.getValue();

            Consigne = (pot * 150.0f) / 4095.0f;   // 3.3 V --> 4095 --> 150C, on lit une valeur entre 0-4095 et on veut la mettre entre 0-150C

            cond = true;
            sem = true; // Les deux conversions sont terminées : température + consigne

            __HAL_ADC_CLEAR_FLAG(Adc1.getHandle(), ADC_FLAG_EOC);

            // Ici on ne relance pas directement l'ADC.
            // On laisse le main traiter les valeurs, puis le main relancera l'ADC.
        }
    }
}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
