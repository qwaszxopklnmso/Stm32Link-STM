#include "stm32f10x.h"
#include "Buzz.h"
#include "Tick.h"

// 4-voice buzzer driver for chiptune / MIDI playback.
//   V1 TIM2_CH1 PA0   (hardware PWM)
//   V2 TIM1_CH1 PA8   (hardware PWM, advanced timer -> needs MOE)
//   V3 TIM3_CH1 PA6   (hardware PWM)
//   V4 PA5            (software square: TIM4 update ISR @16kHz, phase accumulator)
// All hardware timers run at 72MHz; PSC=71 -> 1MHz -> ARR = 1e6/f - 1.
//
// Passive buzzers only. The low-level-triggered piezo and the electromagnetic
// type are both driven by the same square wave:
//   * 50% duty (BUZZ_DUTY_PERCENT) is the loudest square wave; a coil sees the
//     half-rail average, which is normal for electromagnetic buzzers.
//   * silence = the inactive level, never a steady "on" level.
//
// TRIGGER POLARITY is per voice (Buzz_SetTriggerLow / BUZZ_TRIG_LOW_Vx):
//   1 = 低电平触发: pin LOW = ON, idle HIGH  (new piezo module / PNP driver)
//   0 = 高电平触发: pin HIGH = ON, idle LOW  (electromagnetic on the old NPN stage)
// Default: V1-V3 high (electromagnetic on NPN), V4 low (piezo beeper).

static uint32_t hwEnd[3];
static uint8_t  hwPlaying[3];

// 1 = low-level trigger, 0 = high-level trigger, one per voice
static uint8_t trigLow[BUZZ_VOICES] = {
	BUZZ_TRIG_LOW_V1, BUZZ_TRIG_LOW_V2, BUZZ_TRIG_LOW_V3, BUZZ_TRIG_LOW_V4
};

// voice 4 software square
static volatile uint32_t swInc = 0;        // phase increment per 16kHz tick
static volatile uint32_t swAcc = 0;
static volatile uint16_t swRemaining = 0;  // ISR ticks left
static volatile uint8_t  swLevel = 0;

static TIM_TypeDef *Buzz_Tim(uint8_t v) {
	return (v == 0) ? TIM2 : (v == 1) ? TIM1 : TIM3;
}

// Drive PA5 (voice 4) to the active or inactive level of its trigger polarity.
static void Buzz_Pin4(uint8_t active) {
	if (active) {
		if (trigLow[3]) GPIO_ResetBits(GPIOA, GPIO_Pin_5);   // low-level trigger: low = ON
		else GPIO_SetBits(GPIOA, GPIO_Pin_5);                 // high-level trigger: high = ON
	} else {
		if (trigLow[3]) GPIO_SetBits(GPIOA, GPIO_Pin_5);
		else GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	}
}

// Silence one voice: PWM channel -> CCR 0 (inactive level), PA5 -> inactive level.
static void Buzz_Off(uint8_t v) {
	if (v >= 3) {
		swRemaining = 0;
		Buzz_Pin4(0);
		return;
	}
	TIM_SetCompare1(Buzz_Tim(v), 0);
	hwPlaying[v] = 0;
}

// ---- PCM sample playback (takes over V1 pin PA0 / TIM2) ----
// 4-bit IMA ADPCM @8kHz (2 samples per byte, low nibble first), decoded on the fly
// and written to TIM2 CCR (ARR=255 -> 281kHz carrier, the transducer low-passes it
// back into audio). Needs a wideband transducer: electromagnetic/coil works, a
// piezo disc only makes single tones and will not reproduce this.
// Sample clock = TIM4 ISR @16kHz, every 2nd tick = 8kHz.
static const uint16_t ImaStep[89] = {
	7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,
	107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,
	724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,
	3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,
	15289,16818,18500,20350,22385,24623,27086,29794,32767
};
static const int8_t ImaAdj[8] = {-1,-1,-1,-1,2,4,6,8};

static const uint8_t *pcmData = 0;
static uint16_t pcmLen = 0, pcmIdx = 0;
static uint8_t pcmNibHi = 0, pcmStepIdx = 0, pcmActive = 0;
static int pcmPred = 0;

static void Tim2Restore(void) {
	TIM_Cmd(TIM2, DISABLE);
	TIM_SetAutoreload(TIM2, 1000 - 1);
	TIM_PrescalerConfig(TIM2, 72 - 1, TIM_PSCReloadMode_Immediate);
	TIM_SetCompare1(TIM2, 0);            // CCR 0 = inactive level = silent
	TIM_GenerateEvent(TIM2, TIM_EventSource_Update);
	TIM_Cmd(TIM2, ENABLE);
}

void Buzz_PlayPcm(const uint8_t *data, uint16_t len) {
	if (!data || !len) return;
	Buzz_Off(0);                                 // stop the V1 note
	Buzz_Off(3);                                 // PCM suspends voice 4: silence it first
	TIM_Cmd(TIM2, DISABLE);
	TIM_SetAutoreload(TIM2, 256 - 1);            // 281kHz carrier
	TIM_PrescalerConfig(TIM2, 0, TIM_PSCReloadMode_Immediate);
	TIM_SetCompare1(TIM2, 128);                  // 50% carrier = symmetric, silent
	TIM_GenerateEvent(TIM2, TIM_EventSource_Update);
	TIM_Cmd(TIM2, ENABLE);
	pcmData = data; pcmLen = len; pcmIdx = 0;
	pcmNibHi = 0; pcmStepIdx = 0; pcmPred = 0; pcmActive = 1;
}

uint8_t Buzz_PcmActive(void) { return pcmActive; }

void Buzz_Init(void) {
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef tb;
	TIM_OCInitTypeDef oc;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

	gi.GPIO_Mode = GPIO_Mode_AF_PP;
	gi.GPIO_Speed = GPIO_Speed_50MHz;
	// PA0 (TIM2_CH1), PA8 (TIM1_CH1), PA6 (TIM3_CH1)
	gi.GPIO_Pin = GPIO_Pin_0; GPIO_Init(GPIOA, &gi);
	gi.GPIO_Pin = GPIO_Pin_8; GPIO_Init(GPIOA, &gi);
	gi.GPIO_Pin = GPIO_Pin_6; GPIO_Init(GPIOA, &gi);
	// PA5 = voice 4, plain push-pull output
	gi.GPIO_Mode = GPIO_Mode_Out_PP;
	gi.GPIO_Pin = GPIO_Pin_5; GPIO_Init(GPIOA, &gi);
	Buzz_Pin4(0);                         // idle level of its trigger polarity

	tb.TIM_ClockDivision = TIM_CKD_DIV1;
	tb.TIM_CounterMode = TIM_CounterMode_Up;
	tb.TIM_RepetitionCounter = 0;
	tb.TIM_Prescaler = 72 - 1;            // 1MHz
	tb.TIM_Period = 1000 - 1;

	// TIM2
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseInit(TIM2, &tb);
	TIM_OCStructInit(&oc);
	oc.TIM_OCMode = TIM_OCMode_PWM1;
	oc.TIM_OCPolarity = trigLow[0] ? TIM_OCPolarity_Low : TIM_OCPolarity_High;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_Pulse = 0;
	TIM_OC1Init(TIM2, &oc);
	TIM_Cmd(TIM2, ENABLE);

	// TIM1 (advanced: enable main output)
	TIM_InternalClockConfig(TIM1);
	TIM_TimeBaseInit(TIM1, &tb);
	oc.TIM_OCPolarity = trigLow[1] ? TIM_OCPolarity_Low : TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &oc);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);

	// TIM3
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseInit(TIM3, &tb);
	oc.TIM_OCPolarity = trigLow[2] ? TIM_OCPolarity_Low : TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &oc);
	TIM_Cmd(TIM3, ENABLE);

	// TIM4: pure timebase for voice 4 ISR, 72MHz/4500 = 16kHz
	TIM_InternalClockConfig(TIM4);
	tb.TIM_Prescaler = 0;
	tb.TIM_Period = 4500 - 1;
	TIM_TimeBaseInit(TIM4, &tb);
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	NVIC_InitTypeDef nv;
	nv.NVIC_IRQChannel = TIM4_IRQn;
	nv.NVIC_IRQChannelCmd = ENABLE;
	nv.NVIC_IRQChannelPreemptionPriority = 2;
	nv.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nv);
	TIM_Cmd(TIM4, ENABLE);
}

// Trigger polarity per voice: low != 0 -> 低电平触发, 0 -> 高电平触发.
// Safe to call while playing: the voice is silenced first.
void Buzz_SetTriggerLow(uint8_t voice, uint8_t low) {
	if (voice >= BUZZ_VOICES) return;
	trigLow[voice] = low ? 1 : 0;
	if (voice < 3) {
		TIM_TypeDef *tim = Buzz_Tim(voice);
		TIM_SetCompare1(tim, 0);             // stop this voice first
		TIM_OC1PolarityConfig(tim, trigLow[voice] ? TIM_OCPolarity_Low : TIM_OCPolarity_High);
		hwPlaying[voice] = 0;
	} else {
		swRemaining = 0;
		Buzz_Pin4(0);
	}
}

uint8_t Buzz_GetTriggerLow(uint8_t voice) {
	return (voice < BUZZ_VOICES) ? trigLow[voice] : 1;
}

// freq == 0 -> silence this voice
void Buzz_Note(uint8_t voice, uint16_t freq, uint16_t ms) {
	if (voice >= BUZZ_VOICES) return;
	if (pcmActive && (voice == 0 || voice >= 3)) return;  // PCM owns TIM2 / suspends V4

	if (freq == 0 || ms == 0) {
		Buzz_Off(voice);
		return;
	}

	if (voice >= 3) {
		// software voice: phase accumulator at 16kHz, toggle when bit31 flips
		swInc = (uint32_t)((uint64_t)freq * 4294967296u / 16000u);
		swAcc = 0;
		swLevel = 1;                              // start on the active half
		Buzz_Pin4(1);
		swRemaining = (uint16_t)(ms * 16u > 65000u ? 65000u : ms * 16u);
		return;
	}

	TIM_TypeDef *tim = Buzz_Tim(voice);
	uint32_t arr = 1000000u / freq - 1u;
	TIM_SetAutoreload(tim, (uint16_t)arr);
	TIM_SetCompare1(tim, (uint16_t)(arr * BUZZ_DUTY_PERCENT / 100u));
	TIM_GenerateEvent(tim, TIM_EventSource_Update);
	hwEnd[voice] = Tick_GetMs() + ms;
	hwPlaying[voice] = 1;
}

// Short confirmation tone on the piezo voice (default V4/PA5) at its resonance.
void Buzz_Beep(void) {
	Buzz_Note(BUZZ_PIEZO_VOICE, BUZZ_PIEZO_HZ, BUZZ_BEEP_MS);
}

void Buzz_Update(void) {
	uint32_t now = Tick_GetMs();
	for (uint8_t v = 0; v < 3; v++) {
		if (hwPlaying[v] && (int32_t)(now - hwEnd[v]) >= 0) Buzz_Off(v);
	}
}

void TIM4_IRQHandler(void) {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if (pcmActive) {
			// one ADPCM sample per tick (8kHz), low nibble first
			if (pcmIdx < pcmLen) {
				uint8_t nib = pcmNibHi ? (pcmData[pcmIdx] >> 4) : (pcmData[pcmIdx] & 0x0F);
				int step = ImaStep[pcmStepIdx];
				int d = step >> 3;
				if (nib & 4) d += step;
				if (nib & 2) d += step >> 1;
				if (nib & 1) d += step >> 2;
				if (nib & 8) pcmPred -= d; else pcmPred += d;
				if (pcmPred > 32767) pcmPred = 32767; else if (pcmPred < -32768) pcmPred = -32768;
				int ni = pcmStepIdx + ImaAdj[nib & 7];
				if (ni < 0) ni = 0; else if (ni > 88) ni = 88;
				pcmStepIdx = (uint8_t)ni;
				TIM2->CCR1 = (uint8_t)(((pcmPred >> 8) + 128) & 0xFF);
				pcmNibHi ^= 1;
				if (!pcmNibHi) pcmIdx++;
			} else {
				pcmActive = 0;
				TIM2->CCR1 = 0;
				Tim2Restore();
			}
			return;                               // voice 4 suspended during PCM
		}
		if (swRemaining) {
			uint32_t prev = swAcc;
			swAcc += swInc;
			if ((swAcc ^ prev) & 0x80000000u) {        // phase MSB flipped -> half period
				swLevel = (uint8_t)!swLevel;
				Buzz_Pin4(swLevel);
			}
			swRemaining--;
			if (!swRemaining) Buzz_Pin4(0);            // end of note: idle level
		}
	}
}
