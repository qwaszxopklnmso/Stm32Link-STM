#ifndef __BUZZ_H
#define __BUZZ_H

#include "stm32f10x.h"

#define BUZZ_VOICES 4

// ---- Trigger polarity per voice -----------------------------------------
// 1 = 低电平触发 (pin LOW = drive, idle HIGH) — the passive piezo module.
// 0 = 高电平触发 (pin HIGH = drive, idle LOW) — an electromagnetic buzzer
//     on an NPN low-side stage.
// Default: V1-V3 high (electromagnetic on NPN), V4 low (piezo beeper).
// The four voices can be mixed freely.
#ifndef BUZZ_TRIG_LOW_V1
#define BUZZ_TRIG_LOW_V1 0
#endif
#ifndef BUZZ_TRIG_LOW_V2
#define BUZZ_TRIG_LOW_V2 0
#endif
#ifndef BUZZ_TRIG_LOW_V3
#define BUZZ_TRIG_LOW_V3 0
#endif
#ifndef BUZZ_TRIG_LOW_V4
#define BUZZ_TRIG_LOW_V4 1
#endif

// ---- Drive duty ----------------------------------------------------------
// Percent of one period spent at the active level. 50 = symmetric square wave,
// the loudest drive. A coil still sees the half-rail average (normal for these
// buzzers); what matters is that silence is the inactive level, not a steady on.
// Lower it (e.g. 30) if a direct-driven piezo is too loud.
#ifndef BUZZ_DUTY_PERCENT
#define BUZZ_DUTY_PERCENT 50
#endif

// ---- Piezo beeper (V4 / PA5) --------------------------------------------
// The song leaves V4 empty, so it doubles as a low-level-trigger piezo beeper.
// It beeps at the piezo's resonant frequency; the melody stays on V1-V3 (which
// can also be piezo). Change the macro if the beeper is on another voice.
#ifndef BUZZ_PIEZO_VOICE
#define BUZZ_PIEZO_VOICE 3        // 0-based: 3 = V4 = PA5
#endif
#ifndef BUZZ_PIEZO_HZ
#define BUZZ_PIEZO_HZ 4000        // typical 3-4kHz; MH-FMD modules are often 2.7kHz
#endif
#ifndef BUZZ_BEEP_MS
#define BUZZ_BEEP_MS 80
#endif

void Buzz_Init(void);
void Buzz_Note(uint8_t voice, uint16_t freq, uint16_t ms);   // freq 0 = silence
void Buzz_Update(void);
void Buzz_PlayPcm(const uint8_t *data, uint16_t len);        // takes over V1 pin
uint8_t Buzz_PcmActive(void);
void Buzz_Beep(void);                                        // piezo confirmation on V4

// Runtime polarity switch (the same thing as BUZZ_TRIG_LOW_Vx, without a rebuild).
void Buzz_SetTriggerLow(uint8_t voice, uint8_t low);   // 1 = low trigger, 0 = high
uint8_t Buzz_GetTriggerLow(uint8_t voice);

#endif
