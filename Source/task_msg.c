// task_msg.c

//message format:
//33>04: Hallo Welt!\n

#include "task_init.h"
#include "sema_msg.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "hardware.h"
#include "task_database.h"

void Task_msg1 (void* database_ptr)
{
	database *db = (database*) database_ptr;
	int receiverID_int, senderID_int; 
	char receivedChars[BUFFER_SIZE];
	char error_msg[37];
	char receiverID_char[3], senderID_char[3];
	int size = 0;
	db_data *receiver_db_entry;
	
	for(;;)
	{
		//incomig msg from USART1
		if (xSemaphoreTake(sema1_MSG, portMAX_DELAY))
		{
			//receive buffer
			debug3("Task_msg1")
			size = xMessageBufferReceive(msg_USART1_MSG, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			receivedChars[size+1] = '\0';
			
			if (size)
			{
				senderID_int = atoi(strtok(receivedChars, ">"));
				receivedChars[strlen(receivedChars)] = '>';
				receiverID_int = atoi(strtok(NULL, ":"));
				receivedChars[strlen(receivedChars)] = ':';
				
				//search ID in database and save into receiverID_interface
				debug2("MSG1: search receiver ID in database")
				receiver_db_entry = find_pc(db, receiverID_int);
				
				//PC not found in database
				if (!receiver_db_entry)
				{
					//send back "host unreachable"
					//create msg:
					debug2("MSG1: Host not found in the database!");
					error_msg[0] = '\0';
					strcat(error_msg, "00>");
					strcat(error_msg, ID_to_string(senderID_char, senderID_int));
					strcat(error_msg, ": Error: ");
					strcat(error_msg, ID_to_string(receiverID_char, receiverID_int));
					strcat(error_msg, " Host Unreachable!\n\r");
					
					//send message back:
					if(!(senderID_int == 0))
					{
						send(Usart1, error_msg, strlen(error_msg));
					}
					else
					{
						debug1("Error in MSG1: sender = 0")
					}
				}
				//send out USART1
				else if (receiver_db_entry->interface == 1)
				{
					debug2("MSG1: message sent on usart1")
					send(Usart1, receivedChars, size);
				}
				//send out USART2
				else if (receiver_db_entry->interface == 2)
				{
					debug2("MSG1: message sent on usart2")
					
					//check if message was too long
					if (size >= BUFFER_SIZE-5)
					{
						strcat(receivedChars, "\n\r");
						size += 2;
						if (size >= BUFFER_SIZE-3)
						{
							send(Usart2, "(The sent message was to long, ", 31);
							send(Usart2, "it may be missing text)\n\r", 25);
						}
					}
					
					send(Usart2, "\x1b[33;1m", 8);
					send(Usart2, receivedChars, size);
					send(Usart2, "\x1b[0m", 5);
				}
				//send out USART6
				else if (receiver_db_entry->interface == 6)
				{
					debug2("MSG1: message sent on usart6")
					send(Usart6, receivedChars, size);
				}
				else
				{
					debug1("error in Task_msg1: interface incorrect")
				}
			}
		}
	}
}


void Task_msg2 (void* database_ptr)
{
	database *db = (database*) database_ptr;
	int receiverID_int, senderID_int;
	char no_db_enrty[] = "Message could not be sent\n\r";
	char receivedChars[BUFFER_SIZE-1];
	char error_msg[37];
	char receiverID_char[3], senderID_char[3];
	int size = 0;
	db_data *receiver_db_entry;

	for(;;)
	{
		//incomig msg from USART2
		if (xSemaphoreTake(sema2_MSG, portMAX_DELAY))
		{
			debug3("Task_msg2")
			//receive buffer
			size = xMessageBufferReceive(msg_USART2_MSG, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			
			if (size)
			{
				senderID_int = atoi(strtok(receivedChars, ">"));
				receivedChars[strlen(receivedChars)] = '>';
				receiverID_int = atoi(strtok(NULL, ":"));
				receivedChars[strlen(receivedChars)] = ':';
				
				//search ID in database and save into receiverID_interface
				//receiverID_interface = db_id_to_interface(receiverID_int);
				receiver_db_entry = find_pc(db, receiverID_int);

				debugVar(receivedChars)
				
				//PC not found in database
				if (!receiver_db_entry)
				{
					debug2("MSG2: Host not found in the database!");
					//send back "host unreachable"
					//create msg:
					error_msg[0] = '\0';
					strcat(error_msg, "00>");
					strcat(error_msg, ID_to_string(senderID_char, senderID_int));
					strcat(error_msg, ": Error: ");
					strcat(error_msg, ID_to_string(receiverID_char, receiverID_int));
					strcat(error_msg, " Host Unreachable!\n\r");

					//send message back:
					debug2("MSG2: Did not find receiverID in database")
					debug_timer
					debug2("MSG2: Send the message back to sender")
					send(Usart2, error_msg, strlen(error_msg));
					send(Usart2, no_db_enrty, sizeof(no_db_enrty));
				}
				//send out USART1
				else if (receiver_db_entry->interface == 1)
				{
					send(Usart1, receivedChars, size);
				}
				//send out USART2
				else if (receiver_db_entry->interface == 2)
				{					
					debug2("MSG6: message sent on usart2")
					
					//check if message was too long
					if (size >= BUFFER_SIZE-5)
					{
						strcat(receivedChars, "\n\r");
						size += 2;
						if (size >= BUFFER_SIZE-5)
						{
							send(Usart2, "(The sent message was to long, ", 31);
							send(Usart2, "it may be missing text)\n\r", 25);
						}
					}
					
					send(Usart2, "\x1b[33;1m", 8);
					send(Usart2, receivedChars, size);
					send(Usart2, "\x1b[0m", 5);
				}
				//send out USART6
				else if (receiver_db_entry->interface == 6)
				{
					send(Usart6, receivedChars, size);
				}
				else
				{
					debug1("error in Task_msg2")
				}
			}
		}
	}
}


void Task_msg6 (void* database_ptr)
{
	database *db = (database*) database_ptr;
	int receiverID_int, senderID_int; 
	char receivedChars[BUFFER_SIZE];
	char error_msg[37];
	char receiverID_char[3], senderID_char[3];
	int size = 0;
	db_data *receiver_db_entry;
	
	for(;;)
	{
		//incomig msg from USART1
		if (xSemaphoreTake(sema6_MSG, portMAX_DELAY))
		{
			//receive buffer
			debug3("Task_msg6")			
			size = xMessageBufferReceive(msg_USART6_MSG, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			
			char sizes[3];
			ID_to_string(sizes, size);
			debugVar(sizes)
			debugVar(receivedChars)
			
			if (size)
			{
				senderID_int = atoi(strtok(receivedChars, ">"));
				receivedChars[strlen(receivedChars)] = '>';
				receiverID_int = atoi(strtok(NULL, ":"));
				receivedChars[strlen(receivedChars)] = ':';
				
				//search ID in database and save into receiverID_interface
				debug2("MSG6: search receiver ID in database")
				receiver_db_entry = find_pc(db, receiverID_int);
				
				//PC not found in database
				if (!receiver_db_entry)
				{
					//send back "host unreachable"
					//create msg:
					debug2("MSG6: Host not found in the database!");
					error_msg[0] = '\0';
					strcat(error_msg, "00>");
					strcat(error_msg, ID_to_string(senderID_char, senderID_int));
					strcat(error_msg, ": Error: ");
					strcat(error_msg, ID_to_string(receiverID_char, receiverID_int));
					strcat(error_msg, " Host Unreachable!\n\r");
					
					//send message back:
					if(!(senderID_int == 0))
					{
						send(Usart6, error_msg, strlen(error_msg));
					}
					else
					{
						debug1("Error in MSG6: sender = 0")
					}
				}
				//send out USART1
				else if (receiver_db_entry->interface == 1)
				{
					debug2("MSG6: message sent on usart1")
					send(Usart1, receivedChars, size);
				}
				//send out USART2
				else if (receiver_db_entry->interface == 2)
				{
					debug2("MSG6: message sent on usart2")
					
					//check if message was too long
					if (size >= BUFFER_SIZE-5)
					{
						strcat(receivedChars, "\n\r");
						size += 2;
						if (size >= BUFFER_SIZE-5)
						{
							send(Usart2, "(The sent message was to long, ", 31);
							send(Usart2, "it may be missing text)\n\r", 25);
						}
					}
					
					send(Usart2, "\x1b[33;1m", 8);
					send(Usart2, receivedChars, size);
					send(Usart2, "\x1b[0m", 5);
				}
				//send out USART6
				else if (receiver_db_entry->interface == 6)
				{
					debug2("MSG6: message sent on usart6")
					send(Usart6, receivedChars, size);
				}
				else
				{
					debug1("error in Task_msg6: interface incorrect")
				}
			}
		}
	}
}

