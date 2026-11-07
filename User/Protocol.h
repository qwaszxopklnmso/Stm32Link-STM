#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f10x.h"

// Serial line protocol over USART1 (115200 8N1, lines end with '\n', 40-byte buffer).
//   MC -> STM32 : "L<1-3>:<0|1>" set LED | "T:<0|1>" transmitter | "N<v>:<freq>,<ms>" note
//                 | "Y<1-4>:<0|1>" / "Y:<0|1>" trigger polarity
//                 | "W" PCM clip | "P" ping | "S" status | "M"/"M1"/"M2" offline song
//   STM32 -> MC : "B<1-4>:<v>" button/input | "H" heartbeat | "R" ready | "OK" ping reply
//   (LED state is MC -> STM32 only; the firmware sends no L report.)

void Protocol_Init(void);                // reset line state, redraw OLED state rows, send "R\n"
void Protocol_Poll(void);                // drain Serial RX, parse complete lines
void Protocol_OnKeys(uint8_t keyMask);   // B1: spare; B2: pulse; B3: toggle
void Protocol_UpdateInputs(uint8_t pressedMask); // B4: true level, call every loop

#endif
