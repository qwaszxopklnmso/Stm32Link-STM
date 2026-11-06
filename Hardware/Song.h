#ifndef __SONG_H
#define __SONG_H

#include "stm32f10x.h"

// Offline 4-voice song player, fed by the generated arrays in Song2.c
// (command "M"/"M1"/"M2" over serial; no MC needed once started).
// Also owns OLED row 3, the audio status line: song title, notes, "PCM!".

void Song_Start(uint8_t song);
void Song_Update(void);
void Song_ShowNote(uint16_t f, uint16_t ms);   // row-3 note display, shared with the "N" command

#endif
