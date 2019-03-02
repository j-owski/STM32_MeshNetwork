// task.c
// Create Tasks here

#include "task_init.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware.h"
#include "main.h"


void task_INIT(database *db) {
	//task_usart.c
	xTaskCreate( Task_usart1_RX , "Task_usart1_RX", configMINIMAL_STACK_SIZE, db, 4, NULL );
	xTaskCreate( Task_usart2_RX , "Task_usart2_RX", configMINIMAL_STACK_SIZE, db, 4, NULL );
	xTaskCreate( Task_usart6_RX , "Task_usart6_RX", configMINIMAL_STACK_SIZE, db, 4, NULL );
	xTaskCreate( Task_usart1_TX , "Task_usart1_TX", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( Task_usart2_TX , "Task_usart2_TX", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( Task_usart6_TX , "Task_usart6_TX", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
	
	
	//task_msg.c
	xTaskCreate( Task_msg1 , "Task_msg1", configMINIMAL_STACK_SIZE, db, 2, NULL );
	xTaskCreate( Task_msg2 , "Task_msg2", configMINIMAL_STACK_SIZE, db, 2, NULL );
	xTaskCreate( Task_msg6 , "Task_msg6", configMINIMAL_STACK_SIZE, db, 2, NULL );
	
	//task_hello.c
	xTaskCreate( Task_send1_Hello , "Task_send_Hello", configMINIMAL_STACK_SIZE, db, 1, NULL );
	xTaskCreate( Task_send6_Hello , "Task_send_Hello", configMINIMAL_STACK_SIZE, db, 1, NULL );
	xTaskCreate( Task_rec1_Hello , "Task_rec1_Hello", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate( Task_rec6_Hello , "Task_rec6_Hello", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	
	//taks_database.c
	xTaskCreate( Task_db_update1 , "Task_db_update1", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_delete1 , "Task_db_delete1", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_request1 , "Task_db_update1", configMINIMAL_STACK_SIZE, db, 5, NULL );
	
	xTaskCreate( Task_db_update2 , "Task_db_update2", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_delete2 , "Task_db_delete2", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_request2 , "Task_db_update2", configMINIMAL_STACK_SIZE, db, 5, NULL );
	
	xTaskCreate( Task_db_update6 , "Task_db_update6", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_delete6 , "Task_db_delete6", configMINIMAL_STACK_SIZE, db, 5, NULL );
	xTaskCreate( Task_db_request6 , "Task_db_update6", configMINIMAL_STACK_SIZE, db, 5, NULL );
}
