#include "stm32f10x.h"
#include "Delay.h"

uint32_t NVIC_PriorityGroup_Type = NVIC_PriorityGroup_2;
int8_t NVIC_PriorityGroup_f = 0;
uint8_t Pin2SourcePin(uint16_t Pin)  {
	if (Pin==GPIO_Pin_0) return GPIO_PinSource0;
	else if (Pin==GPIO_Pin_1) return GPIO_PinSource1;
	else if (Pin==GPIO_Pin_2) return GPIO_PinSource2;
	else if (Pin==GPIO_Pin_3) return GPIO_PinSource3;
	else if (Pin==GPIO_Pin_4) return GPIO_PinSource4;
	else if (Pin==GPIO_Pin_5) return GPIO_PinSource5;
	else if (Pin==GPIO_Pin_6) return GPIO_PinSource6;
	else if (Pin==GPIO_Pin_7) return GPIO_PinSource7;
	else if (Pin==GPIO_Pin_8) return GPIO_PinSource8;
	else if (Pin==GPIO_Pin_9) return GPIO_PinSource9;
	else if (Pin==GPIO_Pin_10) return GPIO_PinSource10;
	else if (Pin==GPIO_Pin_11) return GPIO_PinSource11;
	else if (Pin==GPIO_Pin_12) return GPIO_PinSource12;
	else if (Pin==GPIO_Pin_13) return GPIO_PinSource13;
	else if (Pin==GPIO_Pin_14) return GPIO_PinSource14;
	else if (Pin==GPIO_Pin_15) return GPIO_PinSource15;
	else return 0;
}

uint32_t Type2PeriphType(GPIO_TypeDef *GPIOType) {
	if (GPIOType==GPIOA) return RCC_APB2Periph_GPIOA;
	else if (GPIOType==GPIOB) return RCC_APB2Periph_GPIOB;
	else if (GPIOType==GPIOC) return RCC_APB2Periph_GPIOC;
	else if (GPIOType==GPIOD) return RCC_APB2Periph_GPIOD;
	else if (GPIOType==GPIOE) return RCC_APB2Periph_GPIOE;
	else if (GPIOType==GPIOF) return RCC_APB2Periph_GPIOF;
	else if (GPIOType==GPIOG) return RCC_APB2Periph_GPIOG;
	else return 0;
}

uint8_t Type2PortSourceType(GPIO_TypeDef *GPIOType) {
	if (GPIOType==GPIOA) return GPIO_PortSourceGPIOA;
	else if (GPIOType==GPIOB) return GPIO_PortSourceGPIOB;
	else if (GPIOType==GPIOC) return GPIO_PortSourceGPIOC;
	else if (GPIOType==GPIOD) return GPIO_PortSourceGPIOD;
	else if (GPIOType==GPIOE) return GPIO_PortSourceGPIOE;
	else if (GPIOType==GPIOF) return GPIO_PortSourceGPIOF;
	else if (GPIOType==GPIOG) return GPIO_PortSourceGPIOG;
	else return 0;
}

uint32_t Pin2EXTILine(uint16_t Pin) {
		if (Pin==GPIO_Pin_0) return EXTI_Line0;
	else if (Pin==GPIO_Pin_1) return EXTI_Line1;
	else if (Pin==GPIO_Pin_2) return EXTI_Line2;
	else if (Pin==GPIO_Pin_3) return EXTI_Line3;
	else if (Pin==GPIO_Pin_4) return EXTI_Line4;
	else if (Pin==GPIO_Pin_5) return EXTI_Line5;
	else if (Pin==GPIO_Pin_6) return EXTI_Line6;
	else if (Pin==GPIO_Pin_7) return EXTI_Line7;
	else if (Pin==GPIO_Pin_8) return EXTI_Line8;
	else if (Pin==GPIO_Pin_9) return EXTI_Line9;
	else if (Pin==GPIO_Pin_10) return EXTI_Line10;
	else if (Pin==GPIO_Pin_11) return EXTI_Line11;
	else if (Pin==GPIO_Pin_12) return EXTI_Line12;
	else if (Pin==GPIO_Pin_13) return EXTI_Line13;
	else if (Pin==GPIO_Pin_14) return EXTI_Line14;
	else if (Pin==GPIO_Pin_15) return EXTI_Line15;
	else return 0;
}
void Pin_Init(GPIO_TypeDef *GPIOType,uint16_t Pin,GPIOMode_TypeDef Mode) {
	RCC_APB2PeriphClockCmd(Type2PeriphType(GPIOType),ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=Mode;
	GPIO_InitStructure.GPIO_Pin=Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOType,&GPIO_InitStructure);
	//GPIO_SetBits(GPIOType,Pin);
}

void Pin_ON(GPIO_TypeDef *GPIOType,uint16_t Pin) {
	GPIO_ResetBits(GPIOType,Pin);
}
void Pin_OFF(GPIO_TypeDef *GPIOType,uint16_t Pin) {
	GPIO_SetBits(GPIOType,Pin);
}
void Pin_Turn(GPIO_TypeDef *GPIOType,uint16_t Pin) {
	if (GPIO_ReadOutputDataBit(GPIOType,Pin)) GPIO_ResetBits(GPIOType,Pin);
	else GPIO_SetBits(GPIOType,Pin);
}
uint8_t Key_GetNum(GPIO_TypeDef *GPIOType,uint16_t Pin) {
	if (!GPIO_ReadInputDataBit(GPIOType,Pin)) {
		Delay_ms(20);
		while (!GPIO_ReadInputDataBit(GPIOType,Pin));
		Delay_ms(20);
		return 1;
	}
	return 0;
}
void NVIC_PriorityGroup_Init(void) {
	if (!NVIC_PriorityGroup_f) {
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Type);
		NVIC_PriorityGroup_f=1;
	}
	return;
}
uint32_t Pin2EXTI_Line(uint16_t Pin)  {
	if (Pin==GPIO_Pin_0) return EXTI_Line0;
	else if (Pin==GPIO_Pin_1) return EXTI_Line1;
	else if (Pin==GPIO_Pin_2) return EXTI_Line2;
	else if (Pin==GPIO_Pin_3) return EXTI_Line3;
	else if (Pin==GPIO_Pin_4) return EXTI_Line4;
	else if (Pin==GPIO_Pin_5) return EXTI_Line5;
	else if (Pin==GPIO_Pin_6) return EXTI_Line6;
	else if (Pin==GPIO_Pin_7) return EXTI_Line7;
	else if (Pin==GPIO_Pin_8) return EXTI_Line8;
	else if (Pin==GPIO_Pin_9) return EXTI_Line9;
	else if (Pin==GPIO_Pin_10) return EXTI_Line10;
	else if (Pin==GPIO_Pin_11) return EXTI_Line11;
	else if (Pin==GPIO_Pin_12) return EXTI_Line12;
	else if (Pin==GPIO_Pin_13) return EXTI_Line13;
	else if (Pin==GPIO_Pin_14) return EXTI_Line14;
	else if (Pin==GPIO_Pin_15) return EXTI_Line15;
	else return 0;
}
/*
GPIOType:
	GPIOx
Pin:
	GPIO_Pin_x
Mode(EXTI):
	GPIO_Mode_IPU //up
	GPIO_MODE_IPD //down
	GPIO_MODE_IN_FLOATING //down
EXTI_Mode:
	if (EXTI_Mode==0) EXTI_Mode = EXTI_Mode_Interrupt;
	if (EXTI_Mode==1) EXTI_Mode = EXTI_Mode_Event;
EXTI_Trigger:
	EXTI_Trigger_Falling (0)
	EXTI_Trigger_Rising (1)
	EXTI_Trigger_Rising_Falling (2)
IRQn:
	EXTI15_10_IRQn(-512)
SubPriority:
	0-15
PreemptionPriority:
	0-15
EXTI_Pin_Init(GPIOB,GPIO_Pin_13,GPIO_Mode_IPU,GPIO_PinSource13,EXTI_Line13,(EXTIMode_TypeDef)0,(EXTITrigger_TypeDef)0,-512,1,1);
*/
/*
BUG:Some Place use "|" "'to' function" is may a bug
this bug about is null?
*/
void EXTI_Pin_Init(GPIO_TypeDef *GPIOType,uint16_t Pin,
	GPIOMode_TypeDef Mode,uint8_t Source_Pin,uint32_t EXTI_Line,
	EXTIMode_TypeDef EXTI_Mode,EXTITrigger_TypeDef EXTI_Trigger,
	int16_t IRQn,int8_t SubPriority,int8_t PreemptionPriority) {
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(Type2PeriphType(GPIOType),ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=Mode;
	GPIO_InitStructure.GPIO_Pin=Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOType,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(Type2PortSourceType(GPIOType),Source_Pin); // AFIO Setting
	
	if (EXTI_Mode==0) EXTI_Mode = EXTI_Mode_Interrupt;
	else if (EXTI_Mode==1) EXTI_Mode = EXTI_Mode_Event;
	
	if (EXTI_Trigger==0) EXTI_Trigger=EXTI_Trigger_Falling;
	else if (EXTI_Trigger==1) EXTI_Trigger=EXTI_Trigger_Rising;
	else if (EXTI_Trigger==2) EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroup_Init();
	
	if (IRQn==-512) IRQn=EXTI15_10_IRQn;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
	NVIC_Init(&NVIC_InitStructure);
	
	
	return;
}
