// sema_msg.c
// here are the definintion and the initialisation of the globals (sema and msg-buffers found)

#include "sema_msg.h"
#include "hardware.h"

SemaphoreHandle_t sema_db;

//Defining the semaphores
//USART1
SemaphoreHandle_t sema1_IRQ_USART_RX;
SemaphoreHandle_t sema1_USART_TX;
SemaphoreHandle_t sema1_HELLO;
SemaphoreHandle_t sema1_HELLO_ACK;
SemaphoreHandle_t sema1_MSG;
SemaphoreHandle_t sema1_DB_UPDATE;
SemaphoreHandle_t sema1_DB_DELETE;
SemaphoreHandle_t sema1_DB_REQUEST;
SemaphoreHandle_t sema1_HELLO_notify;
//USART2
SemaphoreHandle_t sema2_IRQ_USART_RX;
SemaphoreHandle_t sema2_USART_TX;
SemaphoreHandle_t sema2_HELLO;
SemaphoreHandle_t sema2_HELLO_ACK;
SemaphoreHandle_t sema2_MSG;
SemaphoreHandle_t sema2_DB_UPDATE;
SemaphoreHandle_t sema2_DB_DELETE;
SemaphoreHandle_t sema2_DB_REQUEST;
SemaphoreHandle_t sema2_HELLO_notify;
SemaphoreHandle_t sema2_usart_reset;
//USART6
SemaphoreHandle_t sema6_IRQ_USART_RX;
SemaphoreHandle_t sema6_USART_TX;
SemaphoreHandle_t sema6_HELLO;
SemaphoreHandle_t sema6_HELLO_ACK;
SemaphoreHandle_t sema6_MSG;
SemaphoreHandle_t sema6_DB_UPDATE;
SemaphoreHandle_t sema6_DB_DELETE;
SemaphoreHandle_t sema6_DB_REQUEST;
SemaphoreHandle_t sema6_HELLO_notify;

//Declaration of the messagebuffers
//USART1
MessageBufferHandle_t msg_USART1_DB;
MessageBufferHandle_t msg_USART1_MSG;
MessageBufferHandle_t msg_USART1_TX;
SemaphoreHandle_t sema1_msg_USART_TX;
//USART2
MessageBufferHandle_t msg_USART2_DB;
MessageBufferHandle_t msg_USART2_MSG;
MessageBufferHandle_t msg_USART2_TX;
SemaphoreHandle_t sema2_msg_USART_TX;
//USART6
MessageBufferHandle_t msg_USART6_DB;
MessageBufferHandle_t msg_USART6_MSG;
MessageBufferHandle_t msg_USART6_TX;
SemaphoreHandle_t sema6_msg_USART_TX;

void sema_INIT(void) {
	//Init semaphores
	sema_db = xSemaphoreCreateMutex();
	xSemaphoreGive(sema_db);
	
	//for USART1
	sema1_IRQ_USART_RX = xSemaphoreCreateBinary();
	sema1_USART_TX = xSemaphoreCreateBinary();
	sema1_HELLO =	 xSemaphoreCreateBinary();
	sema1_HELLO_ACK = xSemaphoreCreateBinary();
	sema1_HELLO_notify = xSemaphoreCreateBinary();
	sema1_MSG = xSemaphoreCreateBinary();
	sema1_DB_UPDATE = xSemaphoreCreateBinary();
	sema1_DB_DELETE = xSemaphoreCreateBinary();
	sema1_DB_REQUEST = xSemaphoreCreateBinary();
	
	//for USART2
	sema2_IRQ_USART_RX = xSemaphoreCreateBinary();
	sema2_USART_TX = xSemaphoreCreateBinary();
	sema2_HELLO = xSemaphoreCreateBinary();
	sema2_HELLO_ACK = xSemaphoreCreateBinary();
	sema2_HELLO_notify = xSemaphoreCreateBinary();
	sema2_MSG = xSemaphoreCreateBinary();
	sema2_DB_UPDATE = xSemaphoreCreateBinary();
	sema2_DB_DELETE = xSemaphoreCreateBinary();
	sema2_DB_REQUEST = xSemaphoreCreateBinary();
	sema2_usart_reset = xSemaphoreCreateBinary();
	
	//for USART6
	sema6_IRQ_USART_RX = xSemaphoreCreateBinary();
	sema6_USART_TX = xSemaphoreCreateBinary();
	sema6_HELLO = xSemaphoreCreateBinary();
	sema6_HELLO_ACK = xSemaphoreCreateBinary();
	sema6_HELLO_notify = xSemaphoreCreateBinary();
	sema6_MSG = xSemaphoreCreateBinary();
	sema6_DB_UPDATE = xSemaphoreCreateBinary();
	sema6_DB_DELETE = xSemaphoreCreateBinary();
	sema6_DB_REQUEST = xSemaphoreCreateBinary();
	
	//sema for msg buffer
	//1
	sema1_msg_USART_TX = xSemaphoreCreateBinary();
	xSemaphoreGive(sema1_msg_USART_TX);
	//2
	sema2_msg_USART_TX = xSemaphoreCreateBinary();
	xSemaphoreGive(sema2_msg_USART_TX);
	//6
	sema6_msg_USART_TX = xSemaphoreCreateBinary();
	xSemaphoreGive(sema6_msg_USART_TX);
}

//Initialises the messagebuffers
void msg_INIT(void) {	
	//USART1
	msg_USART1_DB = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART1_MSG = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART1_TX = xMessageBufferCreate (BUFFER_SIZE + 10);
	//USART2
	msg_USART2_DB = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART2_MSG = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART2_TX = xMessageBufferCreate (BUFFER_SIZE + 10);
	//USART6
	msg_USART6_DB = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART6_MSG = xMessageBufferCreate (BUFFER_SIZE + 10);
	msg_USART6_TX = xMessageBufferCreate (BUFFER_SIZE + 10);	
}
