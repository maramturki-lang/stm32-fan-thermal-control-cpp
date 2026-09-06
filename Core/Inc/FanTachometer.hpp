#ifndef FAN_TACHOMETER_HPP
#define FAN_TACHOMETER_HPP

#include "Timer.hpp"
#include "main.hpp"

class STM_FanTachometer : public STM_BASETIMER
{
private:
    uint32_t _channel;

public:
    STM_FanTachometer(TIM_TypeDef* timer, uint32_t channel);

    void init(uint32_t period, uint32_t prescaler);

    void startIT();

    uint32_t readCapture();

    void resetCounter();

    uint32_t getChannel() { return _channel; }
};

#endif
