// usart.c
// Here are the IRQ Interrupt from the USARTs defined
// The connection from the hardware is removed (hardware connection is found in hardware.h)

#include "usart.h"
#include "hardware.h"
#include "sema_msg.h"

//USART IRQ handler
//IRQ interrupt is triggered if a char gets received by the USART
//the IRQ handler gives a sempahore, notifing the usart_receive

void IRQHandler_for_USART1()
{
	long lHigherPriorityTaskWoken = pdFALSE;

	USARTclearInterruptRXNE(USART1);

	xSemaphoreGiveFromISR(sema1_IRQ_USART_RX, &lHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
}

void IRQHandler_for_USART2()
{
	long lHigherPriorityTaskWoken = pdFALSE;
	
	USARTclearInterruptRXNE(USART2);

	xSemaphoreGiveFromISR(sema2_IRQ_USART_RX, &lHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
}

void IRQHandler_for_USART6()
{
	long lHigherPriorityTaskWoken = pdFALSE;
	
	USARTclearInterruptRXNE(USART6);

	xSemaphoreGiveFromISR(sema6_IRQ_USART_RX, &lHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( lHigherPriorityTaskWoken );
}

