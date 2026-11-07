#include "Protocol.h"
#include "Serial.h"
#include "Led.h"
#include "Key.h"
#include "Buzz.h"
#include "Pcm.h"
#include "Song.h"
#include "OLED.h"

// A line is deliberately small: the MC side never sends more than ~24 chars,
// and anything longer is dropped instead of being parsed as a truncated command.
#define LINE_BUF 40

static char lineBuf[LINE_BUF];
static uint8_t lineLen = 0, lineOverflow = 0;
static uint8_t b4Level = 0;   // B4 actual level: 1 while held, 0 on release

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
	OLED_ShowNum(4, 4, b4Level, 1);
	OLED_ShowString(4, 6, "T:");
	OLED_ShowNum(4, 8, Led_TledGet(), 1);
}

// Parse a non-empty decimal field, clamping to max. Returns 0 when no digit
// was consumed; advances *pp past the digits. This never reads past the
// NUL-terminated line, and cannot overflow uint32_t with the 40-byte cap.
static uint8_t Parse_U16(const char **pp, uint16_t max, uint16_t *out) {
	const char *p = *pp;
	uint32_t v = 0;
	uint8_t digits = 0;
	while (*p >= '0' && *p <= '9') {
		if (v <= (uint32_t)max) v = v * 10u + (uint32_t)(*p - '0');
		p++;
		digits++;
	}
	if (!digits) return 0;
	if (v > (uint32_t)max) v = (uint32_t)max;
	*out = (uint16_t)v;
	*pp = p;
	return 1;
}

static uint8_t Is01(char c) {
	return (uint8_t)(c == '0' || c == '1');
}

// Every command validates its exact shape before touching s[2]/s[3]. The old
// chain read those bytes even for a one-character line, which is undefined
// behaviour (the byte happened to be inside lineBuf, but was never checked).
static void Line_Parse(char *s, uint8_t len) {
	// show last command (up to 6 chars) on row 1 (validated below)
	uint8_t sl = 0;
	while (sl < 6 && sl < len && s[sl]) sl++;
	for (uint8_t i = 0; i < 6; i++) OLED_ShowChar(1, 10 + i, i < sl ? s[i] : ' ');

	if (len == 4 && s[0] == 'L' && s[1] >= '1' && s[1] <= '3'
			&& s[2] == ':' && Is01(s[3])) {
		uint8_t ch = (uint8_t)(s[1] - '1');
		uint8_t v = (uint8_t)(s[3] == '1');
		Led_Set(ch, v);
		// Output-only: no L report goes back to MC.
		OLED_ShowLeds();
	} else if (len == 3 && s[0] == 'T' && s[1] == ':' && Is01(s[2])) {
		uint8_t v = (uint8_t)(s[2] == '1');
		if (v && !Led_TledGet()) {
			Buzz_Note(0, 1319, 60);   // V1 confirmation (works with only V1 connected)
			Buzz_Beep();              // V4 piezo confirmation at its 4kHz resonance
		}
		Led_TledSet(v);
		OLED_ShowLeds();
	} else if (len >= 3 && s[0] == 'N') {
		uint8_t ch = 0;
		const char *p;
		if (len >= 4 && s[1] >= '1' && s[1] <= '4' && s[2] == ':') {
			ch = (uint8_t)(s[1] - '1');
			p = s + 3;
		} else if (s[1] == ':') {
			p = s + 2;
		} else {
			return;
		}
		uint16_t f = 0, m = 0;
		if (!Parse_U16(&p, 20000, &f)) return;
		if (*p == ',') {
			p++;
			(void)Parse_U16(&p, 60000, &m);
		}
		if (m == 0) m = 100;
		Buzz_Note(ch, f, m);
		if (ch == 0 && f) Song_ShowNote(f, m);
	} else if (len == 1 && s[0] == 'W') {        // PCM clip on V1 pin (real audio)
		Buzz_PlayPcm(Pcm_Audio, Pcm_Len);
		OLED_ShowString(3, 1, "PCM!         ");
	} else if (len == 4 && s[0] == 'Y' && s[1] >= '1' && s[1] <= '4'
			&& s[2] == ':' && Is01(s[3])) {
		// Y<1-4>:<0/1>  per voice: 1 = low-level trigger, 0 = high-level trigger
		uint8_t ch = (uint8_t)(s[1] - '1');
		uint8_t low = (uint8_t)(s[3] == '1');
		Buzz_SetTriggerLow(ch, low);
		Send_State('Y', (uint8_t)(ch + 1), Buzz_GetTriggerLow(ch));
	} else if (len == 3 && s[0] == 'Y' && s[1] == ':' && Is01(s[2])) {
		// Y:<0/1>  all four voices
		uint8_t low = (uint8_t)(s[2] == '1');
		for (uint8_t i = 0; i < BUZZ_VOICES; i++) Buzz_SetTriggerLow(i, low);
		Serial_SendByte('Y'); Serial_SendByte(':');
		Serial_SendNumber(low); Serial_SendByte('\n');
	} else if (len == 1 && s[0] == 'P') {
		Serial_SendString("OK\n");
	} else if (len == 1 && s[0] == 'S') {
		// LEDs are output-only; report just the input level.
		Send_State('B', 4, b4Level);
	} else if (len >= 1 && s[0] == 'M' && (len == 1 || (len == 2 && (s[1] == '1' || s[1] == '2')))) {
		Song_Start((len == 2 && s[1] == '2') ? 2 : 1);
	}
}

void Protocol_Init(void) {
	lineLen = 0;
	lineOverflow = 0;
	b4Level = 0;
	OLED_ShowLeds();
	Serial_SendString("R\n");   // ready, mod will sync on this
}

void Protocol_Poll(void) {
	while (Serial_Available()) {
		uint8_t c = Serial_ReadByte();
		if (c == '\n' || c == '\r') {
			if (lineLen && !lineOverflow) {
				lineBuf[lineLen] = '\0';
				Line_Parse(lineBuf, lineLen);
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
		if (!(km & (1u << i))) continue;
		if (i == 1) {
			// B2: pulse receivers only.
			Send_State('B', 2, 1);
		} else if (i == 2) {
			// B3: toggle receivers only.
			Send_State('B', 3, 1);
		}
		OLED_ShowLeds();
	}
}

// B4 is a true level, not a toggle: call this every loop with Key_GetPressedMask().
// Holding B4 keeps the level receiver on; releasing it turns the receiver off.
void Protocol_UpdateInputs(uint8_t pressedMask) {
	uint8_t level = (pressedMask & KEYB_4) ? 1 : 0;
	if (level == b4Level) return;
	b4Level = level;
	Send_State('B', 4, b4Level);
	OLED_ShowLeds();
}
