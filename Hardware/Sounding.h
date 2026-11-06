#include "stdint.h"
#ifndef       _BUZZER_H_
#define				_BUZZER_H_
 

void Buzzer_Init(void);
 
void Buzzer_Play(void);
void Buzzer_SetVolume(uint8_t Volume);
 
#endif
