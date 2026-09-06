#ifndef UART_HPP
#define UART_HPP




#include "stm32f4xx_hal.h"
#include "main.hpp"
#include <string>

class STM_UART {
private:
    USART_TypeDef* _uart;
    UART_HandleTypeDef _huart;

public:
    // Constructeur défini directement dans le .hpp
    STM_UART(USART_TypeDef* uart) : _uart(uart), _huart{} {}

    // Initialisation complète et simple
        void init(uint32_t baudRate,
                  uint32_t wordLength = UART_WORDLENGTH_8B,
                  uint32_t stopBits   = UART_STOPBITS_1,
                  uint32_t parity     = UART_PARITY_NONE);

        // Envoi de données (Mode bloquant simple)
        void send(const std::string& message);
        void send(uint8_t* pData, uint16_t size);

        // Réception de données (Mode bloquant avec timeout)
        HAL_StatusTypeDef receive(uint8_t* pBuffer, uint16_t size, uint32_t timeout = 100);

        // Accesseur
        UART_HandleTypeDef* getHandle() { return &_huart; }
    };


#endif
