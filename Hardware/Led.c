#include "Led.h"

static const uint16_t LED_PIN[LED_NUM] = {GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3};
static uint8_t ledState[LED_NUM];
static uint8_t tState = 0;

void Led_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef gi;
	gi.GPIO_Mode = GPIO_Mode_Out_PP;
	gi.GPIO_Speed = GPIO_Speed_50MHz;
	for (uint8_t i = 0; i < LED_NUM; i++) {
		gi.GPIO_Pin = LED_PIN[i];
		GPIO_Init(GPIOA, &gi);
		GPIO_ResetBits(GPIOA, LED_PIN[i]);
		ledState[i] = 0;
	}
	gi.GPIO_Pin = GPIO_Pin_4;         // T-LED (MC -> STM32 transmitter mirror)
	GPIO_Init(GPIOA, &gi);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

void Led_Set(uint8_t idx, uint8_t on) {
	ledState[idx] = on ? 1 : 0;
	if (on) GPIO_SetBits(GPIOA, LED_PIN[idx]);
	else GPIO_ResetBits(GPIOA, LED_PIN[idx]);
}

void Led_Toggle(uint8_t idx) {
	Led_Set(idx, (uint8_t)!ledState[idx]);
}

uint8_t Led_Get(uint8_t idx) {
	return ledState[idx];
}

void Led_TledSet(uint8_t on) {
	tState = on ? 1 : 0;
	if (on) GPIO_SetBits(GPIOA, GPIO_Pin_4);
	else GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

uint8_t Led_TledGet(void) {
	return tState;
}
