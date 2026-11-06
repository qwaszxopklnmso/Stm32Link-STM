#include "stm32f10x.h"
#include "OLED.h"
#include "Serial.h"
#include "Buzz.h"
#include "Key.h"
#include "Tick.h"
#include "Led.h"
#include "Song.h"
#include "Protocol.h"

// WithSTM32 - MC <-> STM32 serial link firmware
// Buzzer voices: V1 PA0 (TIM2)  V2 PA8 (TIM1)  V3 PA6 (TIM3)  V4 PA5 (software)
// Buzzer is low-level triggered by default (piezo); electromagnetic voices can
// be switched to high-level trigger per voice (Buzz_SetTriggerLow / Y command).
// LED1-3: PA1/PA2/PA3   T-LED: PA4   Buttons B1-B4: PB12-PB15 (to GND)
// USART1: PA9(TX)/PA10(RX) -> CH340, 115200 8N1
// Wiring: STM32_Set.md   Line protocol: Protocol.h
// Modules: Led (LED/T-LED)  Song (offline player)  Protocol (serial line handling)

int main(void) {
	Tick_Init();
	OLED_Init();
	Serial_Init();
	Buzz_Init();
	Key_Init();
	Led_Init();
	Protocol_Init();

	OLED_ShowString(1, 1, "WithSTM32");

	uint32_t lastHb = Tick_GetMs();
	uint8_t blink = 0;

	while (1) {
		Protocol_Poll();
		Protocol_OnKeys(Key_Scan());
		Buzz_Update();
		Song_Update();

		uint32_t now = Tick_GetMs();
		if (now - lastHb >= 1000) {
			lastHb = now;
			Serial_SendString("H\n");
			blink = (uint8_t)!blink;
			OLED_ShowString(1, 16, blink ? "*" : " ");
		}
	}
}
