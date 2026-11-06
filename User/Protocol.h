#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f10x.h"

// Serial line protocol over USART1 (115200 8N1, lines end with '\n', 32-byte buffer).
//   MC -> STM32 : "L<1-3>:<0|1>" set LED | "T:<0|1>" transmitter | "N<v>:<freq>,<ms>" note
//                 | "W" PCM clip | "P" ping | "S" status | "M"/"M1"/"M2" offline song
//   STM32 -> MC : "L<1-3>:<v>" LED report | "B<1-4>:<v>" button | "H" heartbeat | "R" ready | "OK" ping reply

void Protocol_Init(void);                // reset line state, redraw OLED state rows, send "R\n"
void Protocol_Poll(void);                // drain Serial RX, parse complete lines
void Protocol_OnKeys(uint8_t keyMask);   // button press events -> B/L reports + local toggles

#endif
