// hardware.h
// Here you can find the prototypes and abstractions of the hardware

#ifndef HARDWARE_H
#define HARDWARE_H

//Defines
#include "stm32f4xx.h"

//Variables
static RCC_ClocksTypeDef RCC_Clocks;

//change the Usart Baudrate
//min 4800
//max 400000
#define USART_SPEED 400000

//Prototypes
void init_hardware (void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nCount);

//USART specific
#define IRQHandler_for_USART1() USART1_IRQHandler(void)
#define IRQHandler_for_USART2() USART2_IRQHandler(void)
#define IRQHandler_for_USART6() USART6_IRQHandler(void)
#define USARTclearInterruptRXNE(USARTx); USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
#define get_the_flag_status_from_usart(USARTx, USART_FLAF_XXX) USART_GetFlagStatus(USARTx, USART_FLAF_XXX)
#define send_data_on_usart(USARTx, one_char); do { while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {} USART_SendData(USARTx, one_char); } while(0);

#define LED_green_on(); GPIO_SetBits (GPIOA, GPIO_Pin_5);
#define LED_green_off(); GPIO_ResetBits (GPIOA, GPIO_Pin_5);
#define LED_green_toggle(); GPIO_ToggleBits (GPIOA, GPIO_Pin_5);

#endif
