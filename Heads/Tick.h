#ifndef __TICK_H
#define __TICK_H

#include "stm32f10x.h"

void Tick_Init(void);
uint32_t Tick_GetMs(void);

#endif
