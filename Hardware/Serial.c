#include "stm32f10x.h"
#include "Serial.h"

// USART1 PA9(TX)/PA10(RX) 115200 8N1.
// RX: ISR pushes bytes into a ring buffer so no byte is lost while the main
// loop is busy driving the OLED (software I2C, blocking).

#define RX_RING_SIZE 128

static volatile uint8_t  rxBuf[RX_RING_SIZE];
static volatile uint16_t rxHead = 0, rxTail = 0;

void Serial_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//TX:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//RX:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte) {
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendString(char *String) {
	uint16_t i;
	for (i = 0; String[i] != '\0' && i < 255; i++) Serial_SendByte((uint8_t)String[i]);
}

void Serial_SendNumber(uint32_t x) {
	uint8_t r[10], i = 0;
	if (!x) { Serial_SendByte('0'); return; }
	while (x) { r[i++] = (uint8_t)(x % 10 + '0'); x /= 10; }
	while (i) Serial_SendByte(r[--i]);
}

uint16_t Serial_Available(void) {
	return (uint16_t)((rxHead - rxTail) & (RX_RING_SIZE - 1));
}

uint8_t Serial_ReadByte(void) {
	uint8_t b = rxBuf[rxTail];
	rxTail = (uint16_t)((rxTail + 1) & (RX_RING_SIZE - 1));
	return b;
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		uint8_t d = (uint8_t)USART_ReceiveData(USART1);
		uint16_t next = (uint16_t)((rxHead + 1) & (RX_RING_SIZE - 1));
		if (next != rxTail) {   // full -> drop byte
			rxBuf[rxHead] = d;
			rxHead = next;
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
