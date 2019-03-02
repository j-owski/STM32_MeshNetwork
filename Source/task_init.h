// task_init.h
// Insert Task Prototypes here
#include "task_database.h"

void task_INIT(database *db);

//task_usart.c
void Task_usart1_RX (void* pvParameters);
void Task_usart2_RX (void* database_ptr);
void Task_usart6_RX (void* pvParameters);
void Task_usart1_TX (void* pvParameters);
void Task_usart2_TX (void* pvParameters);
void Task_usart6_TX (void* pvParameters);

//task_msg.c
void Task_msg1 (void* database_ptr);
void Task_msg2 (void* database_ptr);
void Task_msg6 (void* database_ptr);

//task_hello.c
void Task_send1_Hello (void * database_ptr);
void Task_send6_Hello (void * database_ptr);
void Task_rec1_Hello (void * pvParameters);
void Task_rec6_Hello (void * pvParameters);

//task_database.c
void Task_db_update1( void * database_ptr);
void Task_db_delete1( void * database_ptr );
void Task_db_request1( void * database_ptr );

void Task_db_update2( void * database_ptr);
void Task_db_delete2( void * database_ptr );
void Task_db_request2( void * database_ptr );

void Task_db_update6( void * database_ptr);
void Task_db_delete6( void * database_ptr );
void Task_db_request6( void * database_ptr );
