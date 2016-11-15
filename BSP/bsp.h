#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f10x.h"

typedef enum {
    APB1 = 0,
    APB2
} APBType;

#define RCC_APBxPeriph_PC_USART_IO  RCC_APB2Periph_GPIOA
#define RCC_APBxPeriph_PC_USART		RCC_APB1Periph_USART2
#define PC_USART_TXD				GPIO_Pin_2
#define PC_USART_RXD				GPIO_Pin_3
#define PC_USART_IO					GPIOA
#define PC_USART	                USART2
#define PC_PinRemap					DISABLE
#define PC_USARTAPB					APB1

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void BSP_Init(void);
void adc_config(void);
uint16_t get_adc_value(void);
void USART_Config(u32 baud);

#endif
