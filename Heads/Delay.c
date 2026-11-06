#include "stm32f10x.h"
#include "Tick.h"

// Delay now builds on the SysTick millisecond tick (Tick.c) instead of
// reconfiguring SysTick per call, so it coexists with the tick interrupt.

void Delay_us(uint32_t _us) {
	// coarse busy wait (~72 cycles/us at 72MHz, ~7 cycles per loop iteration)
	volatile uint32_t n = _us * 10;
	while (n--) __NOP();
}

void Delay_ms(uint32_t _ms) {
	uint32_t start = Tick_GetMs();
	while (Tick_GetMs() - start < _ms);
}

void Delay_s(uint32_t _s) {
	while (_s--) Delay_ms(1000);
}
