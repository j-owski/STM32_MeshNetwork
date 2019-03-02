// task_hello.c

#include "task_init.h"
#include "task_database.h"
#include "sema_msg.h"
#include "main.h"

//INTERVAL specified time (in ms) when HELLO packets are sent
#define INTERVAL 500
//ATTEMPTS defines how many HELLO don't have to be ACKed before DB_Delete
#define ATTEMPTS 3

// every "router" sends out HELLOs at an regular interval
// when the HELLO gets acknowledged (ACK) by the other router
// the timer gets reseted
// it also checks if the neighbor is known in the database,
// if not it requests the database of the other router
//
// when the router send out 4 HELLO messages, but does not
// get any ACK the router deletes the database entries of that
// interface and notifies its neigbores

//Task responsible for processing the incoming HELLO messages on USART 1
void Task_send1_Hello( void * database_ptr )
{
	database *db = (database*) database_ptr;
	int count = ATTEMPTS;
	char hello = 0x5;
	char request = 0x14;
	
	for( ;; )
	{
		if(count)
		{
			send(Usart1, &hello, sizeof(hello));
			if (xSemaphoreTake(sema1_HELLO_notify, pdMS_TO_TICKS(INTERVAL)))
			{	
				//received ACK from interface
				//request database from Router on the interface if not yet in database
				if (!find_interface(db, 1))
				{
					debug_timer
					debug2("HEL1: found new neighbor!")
					debug_timer
					debug2("HEL1: request database from neighbor")
					send(Usart1, &request, sizeof(request));
				}
				else if (!find_hop(db, 1))
				{
					send(Usart1, &request, sizeof(request));
				}
				vTaskDelay(pdMS_TO_TICKS(INTERVAL));
				//reset timer
				count = ATTEMPTS;
			}
			//no ACK received
			else
			{
				debug_timer
				count--;
			}
		}
		//counter elapsed
		else
		{
			debug_timer
			if(find_interface(db, 1))
			{
				xSemaphoreGive(sema1_DB_DELETE);
			}
			count = ATTEMPTS;
		}
	}
}

void Task_send6_Hello( void * database_ptr )
{
	database *db = (database*) database_ptr;
	int count = ATTEMPTS;
	char hello = 0x5;
	char request = 0x14;
	
	for( ;; )
	{
		if(count)
		{
			send(Usart6, &hello, sizeof(hello));
			if (xSemaphoreTake(sema6_HELLO_notify, pdMS_TO_TICKS(INTERVAL)))
			{	
				//received ACK from interface
				//request database from Router on the interface if not yet in database
				if (!find_interface(db, 6))
				{
					debug_timer
					debug2("HEL6: found new neighbor!")
					debug_timer
					debug2("HEL6: request database from neighbor")
					send(Usart6, &request, sizeof(request));
				}
				else if (!find_hop(db, 1))
				{
					send(Usart6, &request, sizeof(request));
				}
				vTaskDelay(pdMS_TO_TICKS(INTERVAL));
				//reset timer
				count = ATTEMPTS;
			}
			//no ACK received
			else
			{
				debug_timer
				count--;
			}
		}
		//counter elapsed
		else
		{
			debug_timer
			if(find_interface(db, 6))
			{
				xSemaphoreGive(sema6_DB_DELETE);
			}
			count = ATTEMPTS;
		}
	}
}

//Task responsible for processing the incoming HELLO_ACK message on USART 1
void Task_rec1_Hello( void * pvParameters)
{
	char ack = 0x6;
	
	for( ;; )
	{
		if (xSemaphoreTake(sema1_HELLO_ACK, portMAX_DELAY))
		{
			send(Usart1, &ack, sizeof(ack));
		}
	}
}

//Task responsible for processing the incoming HELLO_ACK messages on USART 6
void Task_rec6_Hello( void * pvParameters)
{
	char ack = 0x6;
	
	for( ;; )
	{
		if (xSemaphoreTake(sema6_HELLO_ACK, portMAX_DELAY))
		{
			send(Usart6, &ack, sizeof(ack));
		}
	}
}
