#include "Protocol.h"
#include "Serial.h"
#include "Led.h"
#include "Key.h"
#include "Buzz.h"
#include "Pcm.h"
#include "Song.h"
#include "OLED.h"

#define LINE_BUF 32
static char lineBuf[LINE_BUF];
static uint8_t lineLen = 0, lineOverflow = 0;
static uint8_t b4State = 0;   // B4 latch: redstone broadcast switch, toggled by button

static void Send_State(char tag, uint8_t num, uint8_t val) {
	Serial_SendByte(tag);
	Serial_SendNumber(num);
	Serial_SendByte(':');
	Serial_SendNumber(val);
	Serial_SendByte('\n');
}

static void OLED_ShowLeds(void) {
	OLED_ShowString(2, 1, "L1:");
	OLED_ShowNum(2, 4, Led_Get(0), 1);
	OLED_ShowString(2, 6, "L2:");
	OLED_ShowNum(2, 9, Led_Get(1), 1);
	OLED_ShowString(2, 11, "L3:");
	OLED_ShowNum(2, 14, Led_Get(2), 1);
	OLED_ShowString(4, 1, "B4:");
	OLED_ShowNum(4, 4, b4State, 1);
	OLED_ShowString(4, 6, "T:");
	OLED_ShowNum(4, 8, Led_TledGet(), 1);
}

static void Line_Parse(char *s) {
	// show last command (up to 6 chars) on row 1
	uint8_t sl = 0;
	while (s[sl] && sl < 6) sl++;
	for (uint8_t i = 0; i < 6; i++) OLED_ShowChar(1, 10 + i, i < sl ? s[i] : ' ');
	if (s[0] == 'L' && s[2] == ':' && s[1] >= '1' && s[1] <= '3') {
		uint8_t ch = (uint8_t)(s[1] - '1');
		uint8_t v = (s[3] == '1');
		Led_Set(ch, v);
		Send_State('L', (uint8_t)(ch + 1), Led_Get(ch));
		OLED_ShowLeds();
	} else if (s[0] == 'T' && s[1] == ':') {
		uint8_t v = (s[2] == '1');
		if (v && !Led_TledGet()) {
			Buzz_Note(0, 1319, 60);   // V1 confirmation (works with only V1 connected)
			Buzz_Beep();              // V4 piezo confirmation at its 4kHz resonance
		}
		Led_TledSet(v);
		OLED_ShowLeds();
	} else if (s[0] == 'N') {
		uint8_t ch = 0;
		char *p;
		if (s[1] >= '1' && s[1] <= '4' && s[2] == ':') { ch = (uint8_t)(s[1] - '1'); p = s + 3; }
		else if (s[1] == ':') { p = s + 2; }
		else return;
		uint16_t f = 0, m = 0;
		while (*p >= '0' && *p <= '9') f = (uint16_t)(f * 10 + (*p++ - '0'));
		if (*p == ',') { p++; while (*p >= '0' && *p <= '9') m = (uint16_t)(m * 10 + (*p++ - '0')); }
		if (m == 0) m = 100;
		Buzz_Note(ch, f, m);
		if (ch == 0 && f) Song_ShowNote(f, m);
	} else if (s[0] == 'W') {                    // PCM clip on V1 pin (real audio)
		Buzz_PlayPcm(Pcm_Audio, Pcm_Len);
		OLED_ShowString(3, 1, "PCM!         ");
	} else if (s[0] == 'Y') {                    // buzzer trigger polarity
		// Y<1-4>:<0/1>  per voice: 1 = 低电平触发, 0 = 高电平触发
		// Y:<0/1>       all four voices
		if (s[1] >= '1' && s[1] <= '4' && s[2] == ':') {
			uint8_t ch = (uint8_t)(s[1] - '1');
			uint8_t low = (s[3] == '1');
			Buzz_SetTriggerLow(ch, low);
			Send_State('Y', (uint8_t)(ch + 1), Buzz_GetTriggerLow(ch));
		} else if (s[1] == ':') {
			uint8_t low = (s[2] == '1');
			for (uint8_t i = 0; i < BUZZ_VOICES; i++) Buzz_SetTriggerLow(i, low);
			Serial_SendByte('Y'); Serial_SendByte(':');
			Serial_SendNumber(low); Serial_SendByte('\n');
		}
	} else if (s[0] == 'P') {
		Serial_SendString("OK\n");
	} else if (s[0] == 'S') {
		for (uint8_t i = 0; i < LED_NUM; i++) Send_State('L', (uint8_t)(i + 1), Led_Get(i));
		Send_State('B', 4, b4State);
	} else if (s[0] == 'M') {
		Song_Start((s[1] == '2') ? 2 : 1);   // M/M1/M2 all start the single song (8-bit pipa)
	}
}

void Protocol_Init(void) {
	lineLen = 0;
	lineOverflow = 0;
	OLED_ShowLeds();
	Serial_SendString("R\n");   // ready, mod will sync on this
}

void Protocol_Poll(void) {
	while (Serial_Available()) {
		uint8_t c = Serial_ReadByte();
		if (c == '\n' || c == '\r') {
			if (lineLen && !lineOverflow) {
				lineBuf[lineLen] = '\0';
				Line_Parse(lineBuf);
			}
			lineLen = 0;
			lineOverflow = 0;
		} else if (!lineOverflow) {
			if (lineLen < sizeof(lineBuf) - 1) lineBuf[lineLen++] = (char)c;
			else lineOverflow = 1;
		}
	}
}

void Protocol_OnKeys(uint8_t km) {
	if (!km) return;
	for (uint8_t i = 0; i < KEY_NUM; i++) {
		if (km & (1u << i)) {
			Send_State('B', (uint8_t)(i + 1), 1);   // press event
			if (i < 3) {
				Led_Toggle(i);
				Send_State('L', (uint8_t)(i + 1), Led_Get(i));
			} else {
				b4State = (uint8_t)!b4State;
				Send_State('B', 4, b4State);
			}
			OLED_ShowLeds();
		}
	}
}
