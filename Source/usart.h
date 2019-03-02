// usart.h
// Here you can find the prototypes of the usart-tasks

#ifndef USART_H
#define USART_H
void Task_usart1_RX( void * pvParameters );
void Task_usart2_RX( void * pvParameters );
void Task_usart6_RX( void * pvParameters );

void Task_usart1_TX( void *pvParameters );
void Task_usart2_TX( void *pvParameters );
void Task_usart6_TX( void *pvParameters );
#endif

