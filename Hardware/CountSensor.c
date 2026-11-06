#include "stm32f10x.h"
#include "Delay.h"

uint32_t EXTI_Line;
int32_t CountSensor_Count=0;

void SetEXTI_Line(uint32_t x) {
	EXTI_Line=x;
	return ;
}

int32_t CountSensor_Get(void) {
	return CountSensor_Count;
}
void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line13) == SET) {
		CountSensor_Count ++;
		EXTI_ClearITPendingBit(EXTI_Line13);
		Delay_ms(36);
	}
}
