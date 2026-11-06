#include "stm32f10x.h"
#include "Key.h"
#include "Tick.h"

// 4 push buttons, input pull-up, pressed = LOW
static const GPIO_TypeDef* KEY_PORT[KEY_NUM] = {GPIOB, GPIOB, GPIOB, GPIOB};
static const uint16_t KEY_PIN[KEY_NUM] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

static uint8_t stable[KEY_NUM];      // debounced level, 1 = pressed
static uint32_t lastChange[KEY_NUM];

void Key_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef gi;
	gi.GPIO_Mode = GPIO_Mode_IPU;
	gi.GPIO_Speed = GPIO_Speed_50MHz;
	for (uint8_t i = 0; i < KEY_NUM; i++) {
		gi.GPIO_Pin = KEY_PIN[i];
		GPIO_Init((GPIO_TypeDef*)KEY_PORT[i], &gi);
		stable[i] = 0;
		lastChange[i] = 0;
	}
}

// Returns bitmask KEYB_1..KEYB_4 of keys currently held (debounced level)
uint8_t Key_GetPressedMask(void) {
	uint8_t m = 0;
	for (uint8_t i = 0; i < KEY_NUM; i++) {
		if (stable[i]) m |= (uint8_t)(1u << i);
	}
	return m;
}

// Returns bitmask KEYB_1..KEYB_4 of keys that were just pressed (edge, debounced)
uint8_t Key_Scan(void) {
	uint8_t pressed = 0;
	uint32_t now = Tick_GetMs();
	for (uint8_t i = 0; i < KEY_NUM; i++) {
		uint8_t raw = (GPIO_ReadInputDataBit((GPIO_TypeDef*)KEY_PORT[i], KEY_PIN[i]) == Bit_RESET) ? 1 : 0;
		if (raw != stable[i] && now - lastChange[i] >= 20) {
			stable[i] = raw;
			lastChange[i] = now;
			if (raw) pressed |= (uint8_t)(1u << i);
		}
	}
	return pressed;
}
