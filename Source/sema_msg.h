// sema_msg.h
// in this file the 

#ifndef SEMA_MSG_H
#define SEMA_MSG_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "message_buffer.h"

//Buffer size
#define BUFFER_SIZE 256

void sema_INIT(void);
void msg_INIT(void);

extern SemaphoreHandle_t sema_db;
extern QueueHandle_t myQu1;

//Declaration of the global semaphores
//USART1
extern SemaphoreHandle_t sema1_IRQ_USART_RX;
extern SemaphoreHandle_t sema1_USART_TX;
extern SemaphoreHandle_t sema1_HELLO;
extern SemaphoreHandle_t sema1_HELLO_ACK;
extern SemaphoreHandle_t sema1_MSG;
extern SemaphoreHandle_t sema1_DB_UPDATE;
extern SemaphoreHandle_t sema1_DB_DELETE;
extern SemaphoreHandle_t sema1_DB_REQUEST;
extern SemaphoreHandle_t sema1_HELLO_notify;
//USART2
extern SemaphoreHandle_t sema2_IRQ_USART_RX;
extern SemaphoreHandle_t sema2_USART_TX;
extern SemaphoreHandle_t sema2_HELLO;
extern SemaphoreHandle_t sema2_HELLO_ACK;
extern SemaphoreHandle_t sema2_MSG;
extern SemaphoreHandle_t sema2_DB_UPDATE;
extern SemaphoreHandle_t sema2_DB_DELETE;
extern SemaphoreHandle_t sema2_DB_REQUEST;
extern SemaphoreHandle_t sema2_HELLO_notify;
extern SemaphoreHandle_t sema2_usart_reset;
//USART6
extern SemaphoreHandle_t sema6_IRQ_USART_RX;
extern SemaphoreHandle_t sema6_USART_TX;
extern SemaphoreHandle_t sema6_HELLO;
extern SemaphoreHandle_t sema6_HELLO_ACK;
extern SemaphoreHandle_t sema6_MSG;
extern SemaphoreHandle_t sema6_DB_UPDATE;
extern SemaphoreHandle_t sema6_DB_DELETE;
extern SemaphoreHandle_t sema6_DB_REQUEST;
extern SemaphoreHandle_t sema6_HELLO_notify;

//Declaration of the global messagebuffers
//USART1
extern MessageBufferHandle_t msg_USART1_DB;
extern MessageBufferHandle_t msg_USART1_MSG;
extern MessageBufferHandle_t msg_USART1_TX;
extern SemaphoreHandle_t sema1_msg_USART_TX;
//USART2
extern MessageBufferHandle_t msg_USART2_DB;
extern MessageBufferHandle_t msg_USART2_MSG;
extern MessageBufferHandle_t msg_USART2_TX;
extern SemaphoreHandle_t sema2_msg_USART_TX;
//USART2
extern MessageBufferHandle_t msg_USART6_DB;
extern MessageBufferHandle_t msg_USART6_MSG;
extern MessageBufferHandle_t msg_USART6_TX;
extern SemaphoreHandle_t sema6_msg_USART_TX;

#endif
