#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY_NUM 4
#define KEYB_1 0x01
#define KEYB_2 0x02
#define KEYB_3 0x04
#define KEYB_4 0x08

void Key_Init(void);
uint8_t Key_Scan(void);              // bitmask of keys just pressed (edge)
uint8_t Key_GetPressedMask(void);    // bitmask of keys currently held (level)

#endif
