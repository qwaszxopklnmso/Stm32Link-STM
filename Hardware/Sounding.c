#include "stm32f10x.h"
#include "INITF.h"
#include "OLED.h"


//Every Music Speed(ms)
#define Speed_M_Fast_MaySave 1
#define Speed_M_Fast_Less_Than_0 -1
#define Speed_M_Fast_MayBug 0
#define Speed_M_Fast_Safe 4
//S:Safe
#define SpeedM_Full 2000
#define SpeedM_Full_S 2048
#define SpeedM_2 1000
#define SpeedM_2_S 1024
#define SpeedM_4 500
#define SpeedM_4_S 512
#define SpeedM_8 250
#define SpeedM_8_S 256
#define SpeedM_16 125
#define SpeedM_16_S 128
#define SpeedM_32 62.5
#define SpeedM_32_S 64
#define SpeedM_64 31.25
#define SpeedM_64_S 32
#define SpeedM_128 15.625
#define SpeedM_128_S 16
#define SpeedM_256 7.8125
#define SpeedM_256_S 8
#ifdef Danger_M
#define SpeedM_512 3.90625
#define SpeedM_512_S 4
#define SpeedM_1024 1.653125
#define SpeedM_1024_S 2
#endif
#define Speed SpeedM_16_S


 
#define P	   0    
#define L1     1
#define L1_    2
#define L2     3
#define L2_    4
#define L3     5
#define L4     6
#define L4_    7
#define L5     8
#define L5_    9
#define L6    10
#define L6_   11
#define L7    12
#define M1    13
#define M1_   14
#define M2    15
#define M2_   16
#define M3    17
#define M4    18
#define M4_   19
#define M5    20
#define M5_   21
#define M6    22
#define M6_   23
#define M7    24
#define H1    25
#define H1_   26
#define H2    27
#define H2_   28
#define H3    29
#define H4    30
#define H4_   31
#define H5    32
#define H5_   33
#define H6    34
#define H6_   35
#define H7    36
 

uint16_t FreqBuffer[]=
{
    0,2748,2599,2449,2315,2182,2063,1946,1837,1735,1636,1545,1452,1377,1300,1227,1158,
    1093,1032,973,918,866,818,773,729,688,649,613,578,546,515,486,459,433,409,386,364
};
 
//??????
const uint16_t BadApple[] = {M6,2, P,2, M6,2, M5,2, M6,2, M6,2, M5,2, M6,2,M6,2, M5,2, M6,2, M6,2, H1,2, H2,2, M1,2, M2,2,M6,2, M5,2, M6,2, M6,2, M5,2, M6,2, M6,2, M5,2,M6,2, M5,2, M6,2, H2,2, H1,2, M6,2, M1,2, M1,2,H6,2, H7,2, H1,2, H2,2, H3,2, M6,2, M5,2, M3,2,M6,2, M3,2, M2,2, M1,2, M7,2, H6,2, H7,2, H1,2,H2,2, H3,2, M2,2, M1,2, H7,2, H1,2, H2,2, M3,2,M6,2, M7,2, H1,2, H2,2, H3,2, M6,2, M5,2, M3,2,M6,2, M3,2, M2,2, M1,2, M7,2, H6,2, H7,2, H1,2,H2,2, H3,2, M2,2, M1,2, M7,2, M1,2, M2,2, M3,2,M5,2, M6,2, M3,2, M2,2, M3,2, M2,2, M3,2, M2,2,M3,2, M2,2, M3,2, M2,2, H1,2, H7,2, H5,2, H6,2,M5,2, M6,2, M7,2, M1,2, M2,2, M3,2, M6,2, M5,2,M5,2, M6,2, M3,2, M2,2, M3,2, M2,2, M3,2, M2,2,M3,2, M2,2, M3,2, M2,2, H1,2, H7,2, H5,2, H6,2,M5,2, M6,2, M7,2, M1,2, M2,2, M3,2, M6,2, M5,2,0xFF};
const uint16_t Music1[]=
{
//??,??,
    P,	4,
	P,	4,
	P,	4,
	M6,	2,
	M7,	2,
	
	H1,	4+2,
	M7,	2,
	H1,	4,
	H3,	4,
	
	M7,	4+4+4,
	M3,	2,
	M3,	2,
	
	//2
	M6,	4+2,
	M5,	2,
	M6, 4,
	H1,	4,
	
	M5,	4+4+4,
	M3,	4,
	
	M4,	4+2,
	M3,	2,
	M4,	4,
	H1,	4,
	
	//3
	M3,	4+4,
	P,	2,
	H1,	2,
	H1,	2,
	H1,	2,
	
	M7,	4+2,
	M4_,2,
	M4_,4,
	M7,	4,
	
	M7,	8,
	P,	4,
	M6,	2,
	M7,	2,
	
	//4
	H1,	4+2,
	M7,	2,
	H1,	4,
	H3,	4,
	
	M7,	4+4+4,
	M3,	2,
	M3,	2,
	
	M6,	4+2,
	M5,	2,
	M6, 4,
	H1,	4,
	
	//5
	M5,	4+4+4,
	M2,	2,
	M3,	2,
	
	M4,	4,
	H1,	2,
	M7,	2+2,
	H1,	2+4,
	
	H2,	2,
	H2,	2,
	H3,	2,
	H1,	2+4+4,
	
	//6
	H1,	2,
	M7,	2,
	M6,	2,
	M6,	2,
	M7,	4,
	M5_,4,
	
	
	M6,	4+4+4,
	H1,	2,
	H2,	2,
	
	H3,	4+2,
	H2,	2,
	H3,	4,
	H5,	4,
	
	//7
	H2,	4+4+4,
	M5,	2,
	M5,	2,
	
	H1,	4+2,
	M7,	2,
	H1,	4,
	H3,	4,
	
	H3,	4+4+4+4,
	
	//8
	M6,	2,
	M7,	2,
	H1,	4,
	M7,	4,
	H2,	2,
	H2,	2,
	
	H1,	4+2,
	M5,	2+4+4,
	
	H4,	4,
	H3,	4,
	H3,	4,
	H1,	4,
	
	//9
	H3,	4+4+4,
	H3,	4,
	
	H6,	4+4,
	H5,	4,
	H5,	4,
	
	H3,	2,
	H2,	2,
	H1,	4+4,
	P,	2,
	H1,	2,
	
	//10
	H2,	4,
	H1,	2,
	H2,	2,
	H2,	4,
	H5,	4,
	
	H3,	4+4+4,
	H3,	4,
	
	H6,	4+4,
	H5,	4+4,
	
	//11
	H3,	2,
	H2,	2,
	H1,	4+4,
	P,	2,
	H1,	2,
	
	H2,	4,
	H1,	2,
	H2,	2+4,
	M7,	4,
	
	M6,	4+4+4,
	P,	4,
	
	0xFF	//????
 
};
#define Music BadApple
void Buzzer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    TIM_InternalClockConfig(TIM2);
 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
 
    TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  //??PWM??
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR ??????50,??50
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
    TIM_Cmd(TIM2, ENABLE);
 
}
 
 
void PWM_SetPrescaler(uint16_t Prescaler)
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}
 
 
void Buzzer_SetVolume(uint8_t Volume)
{
	TIM_SetCompare1(TIM2, Volume);
}

void Buzzer_Play()
{
    static uint16_t MusicSelect=0,FreqSelect=0,TimeSelect=0;
		OLED_ShowString(1,1,"Len:");
		OLED_ShowString(2,1,"Music:");
		OLED_ShowChar(1,10,'/');
		OLED_ShowNum(1,11,sizeof(Music)/sizeof(uint16_t)/2-1,5);
    while(Music[MusicSelect]!=0xFF)
    {
				OLED_ShowNum(1,5,MusicSelect/2,5);
				OLED_ShowNum(2,7,Music[MusicSelect],3);
        TIM_Cmd(TIM2, ENABLE);
        FreqSelect=Music[MusicSelect];
        PWM_SetPrescaler(FreqBuffer[FreqSelect]);
 
        TimeSelect=Music[++MusicSelect];
        Delay_ms(Speed/4*TimeSelect);
        MusicSelect++;
 
        Delay_ms(5);
    }
    if(Music[MusicSelect]==0xFF) 
    {
        MusicSelect=0;
        TIM_Cmd(TIM2, DISABLE);
    }
        
 
 
}
 





