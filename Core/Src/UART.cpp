#include "UART.hpp"

void STM_UART::init(uint32_t baudRate, uint32_t wordLength,
		              uint32_t stopBits, uint32_t parity) {
	_huart.Instance = _uart;
	_huart.Init.BaudRate     = baudRate;
    _huart.Init.WordLength   = wordLength;
    _huart.Init.StopBits     = stopBits;
    _huart.Init.Parity       = parity;
    _huart.Init.Mode         = UART_MODE_TX_RX;
    _huart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    _huart.Init.OverSampling = UART_OVERSAMPLING_16;

    // HAL_UART_Init(&_huart);

    if (HAL_UART_Init(&_huart) != HAL_OK)
    {
        Error_Handler();
    }
}
void STM_UART::send(const std::string& message){
    HAL_UART_Transmit(&_huart, (uint8_t*)message.c_str(), message.length(), HAL_MAX_DELAY);
}

void STM_UART::send(uint8_t* pData, uint16_t size) {
    HAL_UART_Transmit(&_huart, pData, size, HAL_MAX_DELAY);
}

HAL_StatusTypeDef STM_UART::receive(uint8_t* pBuffer, uint16_t size, uint32_t timeout) {
    return HAL_UART_Receive(&_huart, pBuffer, size, timeout);
}
