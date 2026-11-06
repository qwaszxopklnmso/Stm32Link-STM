#include "Song.h"
#include "Song2.h"
#include "Buzz.h"
#include "Tick.h"
#include "OLED.h"

// 4 parallel monophone players, one per buzzer voice
typedef struct {
	const uint16_t (*notes)[2];
	uint16_t len;
	uint16_t idx;
	uint32_t nextMs;
	uint8_t active;
} Player;

static Player players[4];

static void Song_Step(uint8_t v) {
	Player *p = &players[v];
	if (p->idx >= p->len) {
		p->active = 0;
		Buzz_Note(v, 0, 0);
		if (v == 0) OLED_ShowString(3, 1, "Song end    ");
		return;
	}
	uint16_t f = p->notes[p->idx][0];
	uint16_t d = p->notes[p->idx][1];
	Buzz_Note(v, f, d);
	p->nextMs = Tick_GetMs() + d + 12;   // small gap for articulation
	p->idx++;
	if (v == 0 && f) Song_ShowNote(f, d);
}

void Song_Start(uint8_t song) {
	(void)song;   // single song: 8-bit pipa (Summer Express removed by user request)
	players[0].notes = S2_V1;  players[0].len = S2_V1_Len;
	players[1].notes = S2_V2;  players[1].len = S2_V2_Len;
	players[2].notes = S2_V3;  players[2].len = S2_V3_Len;
	players[3].notes = S2_V4;  players[3].len = S2_V4_Len;
	OLED_ShowString(3, 1, "Pipa 8bit!   ");
	uint32_t now = Tick_GetMs();
	for (uint8_t v = 0; v < 4; v++) {
		players[v].idx = 0;
		players[v].active = 1;
		players[v].nextMs = now;
		Song_Step(v);
	}
}

void Song_Update(void) {
	uint32_t now = Tick_GetMs();
	for (uint8_t v = 0; v < 4; v++) {
		if (players[v].active && (int32_t)(now - players[v].nextMs) >= 0) Song_Step(v);
	}
}

void Song_ShowNote(uint16_t f, uint16_t ms) {
	OLED_ShowString(3, 1, "N:");
	OLED_ShowNum(3, 3, f, 4);
	OLED_ShowString(3, 7, "Hz ");
	OLED_ShowNum(3, 10, ms, 4);
	OLED_ShowString(3, 14, "ms ");
}
