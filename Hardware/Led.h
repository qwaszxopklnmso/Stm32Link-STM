#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// LED1-3: PA1/PA2/PA3 (active high), T-LED (MC transmitter mirror): PA4
#define LED_NUM 3

void    Led_Init(void);
void    Led_Set(uint8_t idx, uint8_t on);   // idx 0-based
void    Led_Toggle(uint8_t idx);
uint8_t Led_Get(uint8_t idx);
void    Led_TledSet(uint8_t on);
uint8_t Led_TledGet(void);

#endif
