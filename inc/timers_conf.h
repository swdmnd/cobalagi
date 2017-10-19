#ifndef TIMERS_CONF_H_INCLUDED
#define TIMERS_CONF_H_INCLUDED

#include "platform_config.h"

extern __IO uint32_t millis;

void delay_ms(uint32_t);
void RTC_Software_Init();
void Timers_Init();

#endif /* TIMERS_CONF_H_INCLUDED */
