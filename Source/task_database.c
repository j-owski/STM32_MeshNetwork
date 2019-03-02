// task_database.c


#include "main.h"
#include "task_init.h"
#include "task_database.h"
#include "sema_msg.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>


//send an UPDATE out on Usart X with given parameters
void send_intMetric(int UsartX, int pc, int hop, int interface)
{
	debug4("send_intMetric");
	char string[7];
	string[0] = pc / 10 + '0';
	string[1] = pc % 10 + '0';
	string[2] = hop / 10 + '0';
	string[3] = hop % 10 + '0';
	string[4] = interface / 10 + '0';
	string[5] = interface % 10 + '0';
	string[6] = 0x15;
	//send the converted string out of the Usart X
	send(UsartX, string, 7);
	debug_timer
	debugVar(string)
}

// Task that processes the received UPDATE message
void Task_db_update1( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char update[8];
	db_data *db_entry;
	int pc, hop, interface, old_interface, size;
	char request = 0x14;
	int cntr_retransmission = 0;
	
	for(;;)
	{
		if (xSemaphoreTake(sema1_DB_UPDATE, portMAX_DELAY))
		{
			debug3("Task_db_update1")
			
			//receive the string
			size = xMessageBufferReceive(msg_USART1_DB, update, sizeof(update), portMAX_DELAY);
			debug2("received update string:")
			debug_timer
			debugVar(update)
			debug_timer
			
			//check if received data is correct
			if(size != 7)
			{
				//if more than 4 retransmissions, then stop requesting
				if(cntr_retransmission > 3)
				{
					debug1("DB_UPDATE1: database requests not successful")
					xSemaphoreGive(sema2_usart_reset);
				}
				else
				{
					debug_timer
					debug1("DB_UPDATE1: Update string not correct")
					//request a resend
					debug_timer
					vTaskDelay(100);
					debug2("DB-1: request a resend of the database update")
					send(Usart1, &request, sizeof(request));
					cntr_retransmission++;
				}
			}
			else
			{
				//reset retransmission counter
				cntr_retransmission = 0;
				
				//convert string message to integer
				pc = ((update[0]-'0')*10) + (update[1]-'0');
				hop = ((update[2]-'0')*10) + (update[3]-'0');
				old_interface = ((update[4]-'0')*10) + (update[5]-'0');
				interface = 1;
				
				//increment hop
				hop++;
				
				//check if entry for computer id already exists
				debug4("DB_find_pc: search receiver ID")
				db_entry = find_pc(db, pc);
				//already exists
				if(db_entry)
				{
					//check if neighbore learned route through myself -> dont update, since I should know better (split horizon)
					if (old_interface == 1)
					{
						debug2("DB-1: received update dismissed (split horizon)")
					}
					//if new update has a better hop count number than the old database entry -> replace with better metric
					else if (hop < db_entry->hop)
					{
						debug2("DB-1: received better mertric entry from neighbor")
						send(Usart2, "Received a better metric on usart1\n\r", 36);
						db_entry->hop = hop;
						db_entry->interface = interface;
						
						/*propagate trough other interface new route;*/
						//send an UPDATE out from Usart6 with the received metrics
						debug2("DB-1: Propagate new entry on usart6")
						send_intMetric(Usart6, pc, hop, interface);
					}
					else if (hop >= db_entry->hop)
					{
						debug2("DB-1: already got better metric in database")
						debug_timer
						debug2("DB-1: received update dismissed")
					}
				}
				//if PC was not found in the database -> new entry in database
				else
				{
					if (old_interface == 1)
					{
						debug2("DB-1: received update dismissed (split horizon)")
					}
					else if (insertEntry(db, pc, hop, interface))
					{
						debug2("DB-1: new entry")
						//inform others in the network
						debug_timer
						debug2("DB-1: Propagate new entry on usart6")
						send_intMetric(Usart6, pc, hop, interface);
					}
				}
			}
		}
	}
}

// Task that processes the received DELETE message
void Task_db_delete1( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char del = 0x13;
	
	for(;;)
	{
		if (xSemaphoreTake(sema1_DB_DELETE, portMAX_DELAY))
		{
			debug3("Task_db_delete1")
			//receive the string
			
			//if there are entries in the database for an interface 1
			if (find_interface(db, 1))
			{
				debug2("DB-1: delete entries of Interface-1")
				deleteEntriesForInterface(db, 1);
				//propagate DELETE to neighbors
				debug_timer
				debug2("DB-1: Propagate delete on usart6")
				send(Usart6, &del, sizeof(del));
			}
			//else -> no entry -> nothing to delete
		}
	}
}	


// Task that processes the received REQUEST message
// -> send the whole database to the requestor
void Task_db_request1( void * database_ptr )
{
	database *db = (database*) database_ptr;
	db_data *temp;
	
	for(;;)
	{
		if (xSemaphoreTake(sema1_DB_REQUEST, portMAX_DELAY))
		{
			debug3("Task_db_request1")
			
			temp = db->head;
			while(temp)
			{
				debug2("DB-1: send DB Entry")
				send_intMetric(Usart1, temp->pc, temp->hop, temp->interface);
				//small waiting time
				vTaskDelay(100);
				temp = temp->next;
			}
		}
	}
}

// Task that processes the received UPDATE message
void Task_db_update2( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char update[8];
	db_data *db_entry;
	int pc, hop, interface;
	
	for(;;)
	{
		if (xSemaphoreTake(sema2_DB_UPDATE, portMAX_DELAY))
		{
			debug3("Task_db_update2")
			//receive the string
			xMessageBufferReceive(msg_USART2_DB, update, 8, portMAX_DELAY);
			debugVar(update)
			
			//convert string message to integer
			pc = ((update[0]-'0')*10) + (update[1]-'0');
			hop = ((update[2]-'0')*10) + (update[3]-'0');
			interface = ((update[4]-'0')*10) + (update[5]-'0');
			
			//check if entry for computer id already exists
			debug4("DB_find_pc: search receiver ID")
			db_entry = find_pc(db, pc);
			//already exists
			if(db_entry != 0)
			{
				//if new update has a better hop count number than the old database entry -> replace with better metric
        if(hop < db_entry->hop)
				{
					debug2("DB-2: received better mertric entry")
					send(Usart2, "Received a better metric on usart2\n\r", 36);
          db_entry->hop = hop;
          db_entry->interface = interface;
					
          /*propagate trough other interface new route;*/
					//send an UPDATE out from Usart6 with the received metrics
					debug2("DB-2: Propagate new entry on usart1")
					send_intMetric(Usart1, pc, hop, interface);
					debug_timer
					debug2("DB-2: Propagate new entry on usart6")
					send_intMetric(Usart6, pc, hop, interface);
        }
				else
				{
					debug2("DB-2: already better metric in DB")
				}
			}
			//if PC was not found in the database -> new entry in database
			else
			{
				debug2("DB-2: new entry")
				if (insertEntry(db, pc, hop, interface))
				{
					//inform others in the network
					debug_timer
					debug2("DB-2: Propagate new entry on usart1")
					send_intMetric(Usart6, pc, hop, interface);
					debug_timer
					debug2("DB-2: Propagate new entry on usart6")
					send_intMetric(Usart1, pc, hop, interface);
				}
			}
		}
	}
}

// Task that processes the received DELETE message
void Task_db_delete2( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char del = 0x13;
	
	for(;;)
	{
		if (xSemaphoreTake(sema2_DB_DELETE, portMAX_DELAY))
		{
			debug3("Task_db_delete2")
			//receive the string
			
			//if there are entries in the database for an interface 1
			if (find_interface(db, 2))
			{
				debug2("DB-2: delete entries to Interface-2")
				deleteEntriesForInterface(db, 2);
				//propagate DELETE to neighbors
				debug2("DB-2: Propagate delete on usart1")
				send(Usart1, &del, sizeof(del));
				debug2("DB-2: Propagate delete on usart6")
				send(Usart6, &del, sizeof(del));
			}
			//else -> no entry -> nothing to delete
		}
	}
}	


// Task that processes the received REQUEST message
// -> send the whole database to the requestor
void Task_db_request2( void * database_ptr )
{
	database *db = (database*) database_ptr;
	db_data *temp;
	char next_line[] = "\n\r";
	
	for(;;)
	{
		if (xSemaphoreTake(sema2_DB_REQUEST, portMAX_DELAY))
		{
			debug3("Task_db_request2")
			if (db->count)
			{
				send(Usart2, "\x1b[32m", 6);
				temp = db->head;
				while(temp)
				{
					debug2("DB-2: send request")
					send_intMetric(Usart2, temp->pc, temp->hop, temp->interface);
					//small waiting time
					send(Usart2, next_line, sizeof(next_line));
					vTaskDelay(10);
					
					temp = temp->next;
				}
				send(Usart2, "\x1b[0mEnd of your database\n\n\r", 28);
			}
		}
	}
}

// Task that processes the received UPDATE message
void Task_db_update6( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char update[8];
	db_data *db_entry;
	int pc, hop, interface, old_interface, size;
	char request = 0x14;
	int cntr_retransmission = 0;
	
	for(;;)
	{
		if (xSemaphoreTake(sema6_DB_UPDATE, portMAX_DELAY))
		{
			debug3("Task_db_update6")
			//receive the string
			size = xMessageBufferReceive(msg_USART6_DB, update, sizeof(update), portMAX_DELAY);
			debug2("received update string:")
			debug_timer
			debugVar(update)
			
			//check if received data is correct
			if(size != 7)
			{
				//if more than 4 retransmissions, then stop requesting
				if(cntr_retransmission > 1)
				{
					debug1("DB_UPDATE6: database requests not successful")
					xSemaphoreGive(sema2_usart_reset);
				}
				else
				{
					debug_timer
					debug1("DB_UPDATE6: Update string not correct")
					//request a resend
					debug_timer
					debug2("DB-6: request a resend of the database update")
					send(Usart6, &request, sizeof(request));
					cntr_retransmission++;
				}
			}
			else
			{
				//reset retransmission counter
				cntr_retransmission = 0;
				
				//convert string message to integer
				pc = ((update[0]-'0')*10) + (update[1]-'0');
				hop = ((update[2]-'0')*10) + (update[3]-'0');
				old_interface = ((update[4]-'0')*10) + (update[5]-'0');
				interface = 6;
				
				//increment hop
				hop++;
				
				//check if entry for computer id already exists
				debug4("DB_find_pc: search receiver ID")
				db_entry = find_pc(db, pc);
				//already exists
				if(db_entry)
				{
					//check if neighbore learned route through myself -> dont update, since I should know better (split horizon)
					if (old_interface == 6)
					{
						debug2("DB-6: received update dismissed (split horizon)")
					}
					//if new update has a better hop count number than the old database entry -> replace with better metric
					if(hop < db_entry->hop)
					{
						debug2("DB-6: received better mertric entry from neighbor")
						send(Usart2, "Received a better metric on usart6\n\r", 36);
						db_entry->hop = hop;
						db_entry->interface = interface;
						
						/*propagate trough other interface new route;*/
						//send an UPDATE out from Usart6 with the received metrics
						debug2("DB-6: Propagate new entry on usart1")
						send_intMetric(Usart1, pc, hop, interface);
					}
					else if (hop >= db_entry->hop)
					{
						debug2("DB-6: already got better metric in database")
						debug_timer
						debug2("DB-6: received update dismissed")
					}
				}
				//if PC was not found in the database -> new entry in database
				else
				{
					if (old_interface == 6)
					{
						debug2("DB-6: received update dismissed (split horizon)")
					}
					else if (insertEntry(db, pc, hop, interface))
					{
						debug2("DB-6: new entry")
						//inform others in the network
						debug_timer
						debug2("DB-6: Propagate new entry on usart1")
						send_intMetric(Usart1, pc, hop, interface);
					}
				}
			}
		}
	}
}


// Task that processes the received DELETE message
void Task_db_delete6( void * database_ptr )
{
	database *db = (database*) database_ptr;
	char del = 0x13;
	
	for(;;)
	{
		if (xSemaphoreTake(sema6_DB_DELETE, portMAX_DELAY))
		{
			debug3("Task_db_delete6")
			//receive the string
			
			//if there are entries in the database for an interface 1
			if (find_interface(db, 6))
			{
				debug2("DB-6: delete entries of Interface-6")
				deleteEntriesForInterface(db, 6);
				//propagate DELETE to neighbors
				debug_timer
				debug2("DB-6: Propagate delete on usart1")
				send(Usart1, &del, sizeof(del));
			}
			//else -> no entry -> nothing to delete
		}
	}
}	


// Task that processes the received REQUEST message
// -> send the whole database to the requestor
void Task_db_request6( void * database_ptr )
{
	database *db = (database*) database_ptr;
	db_data *temp;
	
	for(;;)
	{
		if (xSemaphoreTake(sema6_DB_REQUEST, portMAX_DELAY))
		{
			debug3("Task_db_request6")
			if (db->count)
			{
				temp = db->head;
				while(temp)
				{
					debug2("DB-6: send request")
					send_intMetric(Usart6, temp->pc, temp->hop, temp->interface);
					//small waiting time
					vTaskDelay(100);
					temp = temp->next;
				}
			}
		}
	}
}

