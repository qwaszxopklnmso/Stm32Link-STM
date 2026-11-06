#include "stm32f10x.h"
#include "Tick.h"

// SysTick 1ms tick (frees TIM1/2/3 for buzzer PWM, TIM4 for voice-4 ISR)
static volatile uint32_t g_ms = 0;

void Tick_Init(void) {
	SysTick->LOAD = 9000 - 1;      // HCLK/8 = 9MHz -> 1ms
	SysTick->VAL = 0;
	SysTick->CTRL = 0x03;          // HCLK/8, interrupt, enable
	NVIC_SetPriority(SysTick_IRQn, 0xC0);   // low priority
}

uint32_t Tick_GetMs(void) {
	return g_ms;
}

void SysTick_Handler(void) {
	g_ms++;
}
