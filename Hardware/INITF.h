#include "stm32f10x.h"
#include "Delay.h"
#ifndef __INITF_H
#define __INITF_H

uint8_t Pin2SourcePin(uint16_t Pin);
uint32_t Type2PeriphType(GPIO_TypeDef *GPIOType);
uint8_t Type2PortSourceType(GPIO_TypeDef *GPIOType);
uint32_t Pin2EXTILine(uint16_t Pin);
void NVIC_PriorityGroup_Init(void);
void Pin_Init(GPIO_TypeDef *GPIOType,uint16_t Pin,GPIOMode_TypeDef Mode);
void Pin_ON(GPIO_TypeDef *GPIOType,uint16_t Pin);//low
void Pin_OFF(GPIO_TypeDef *GPIOType,uint16_t Pin);//high
void Pin_Turn(GPIO_TypeDef *GPIOType,uint16_t Pin);
uint8_t Key_GetNum(GPIO_TypeDef *GPIOType,uint16_t Pin);
void EXTI_Pin_Init(GPIO_TypeDef *GPIOType,uint16_t Pin,
	GPIOMode_TypeDef Mode,uint8_t Source_Pin,uint32_t EXTI_Line,
	EXTIMode_TypeDef EXTI_Mode,EXTITrigger_TypeDef EXTI_Trigger,
	int16_t IRQn,int8_t SubPriority,int8_t PreemptionPriority);

#endif
