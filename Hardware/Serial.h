#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

void     Serial_Init(void);
void     Serial_SendByte(uint8_t Byte);
void     Serial_SendString(char *String);
void     Serial_SendNumber(uint32_t x);
uint16_t Serial_Available(void);
uint8_t  Serial_ReadByte(void);

#endif
