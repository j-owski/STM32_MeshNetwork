// task_usart.c
// Here you can find the usart tasks

#include "main.h"
#include "task_init.h"
#include "hardware.h"
#include "sema_msg.h"
#include <string.h>
#include <stdio.h>
#include "task_database.h"

char help;

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
//X																	RECEIVE																		 X//
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
void Task_usart1_RX (void* database_ptr)
{	
	//database *db = (database*) database_ptr;
	char myChars[BUFFER_SIZE];
	int i = 0;
	
	for( ;; )
  {
		//check for USART interrupt semaphore
		if (xSemaphoreTake(sema1_IRQ_USART_RX, portMAX_DELAY))
		{
			//read the input of the USART to myChar array
			myChars[i] = USART_ReceiveData(USART1);
			//debug3("Task_usart1_RX")
			
			if (myChars[i] == '\0') i=-1;
			
			//Check char for special charakter:
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			//HELLO
			//----------------------------------------------------------------------------------
			if (myChars[i] == 0x5) {xSemaphoreGive(sema1_HELLO_ACK);  i=-1;}
			
			//DB UPDATE
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x15)
			{
				//send char array to message buffer and notify other task
				xMessageBufferSend(msg_USART1_DB, (void *) myChars, ++i, portMAX_DELAY);
				xSemaphoreGive(sema1_DB_UPDATE);
				i = -1;
			}
			
			//HELLO ACK
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x6) {xSemaphoreGive(sema1_HELLO_notify); i=-1;}
			
			//MSG
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0xD)
			{
				//send char array to message buffer and notify other task
				xMessageBufferSend(msg_USART1_MSG, (void *) myChars, ++i, portMAX_DELAY);
				xSemaphoreGive(sema1_MSG);
				i = -1;				
			}

			//DB DELETE
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x13) {xSemaphoreGive(sema1_DB_DELETE); i=-1;}
			
			//DB REQUEST
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x14) {xSemaphoreGive(sema1_DB_REQUEST); i=-1;}
			
			
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			//end of special characters
			
			//increase counter until max buffer size is reached
			if (i < (BUFFER_SIZE - 6) || i == -1)
			{
				i++;
			}
		}
  }
}

void Task_usart2_RX (void* database_ptr)
{	
	database *db = (database*) database_ptr;
	db_data *my_db_entry;
	char myChars[BUFFER_SIZE], msg_buffer[BUFFER_SIZE];
	char input_error_initial[] = "\n\rPlease type a new ID (1..99): ";
	char input_error[] = "Input not correct (see /help)\n\r";
	char id_positiv_notify[] = "Your ID is: XX\n\r";
	char my_ID_int = 0, my_ID_string[2];
	char update_string[] = {'0','0','0','0','0','2', 15};
	char request = 0x14;
	int i = 0, j;
	
	//send inital Message
	send(Usart2, "\033[2J\033[1;1H", 11);
	banner_message();	
	send(Usart2, "\n\rsee /help for further information\n\r", 38);
	send(Usart2, input_error_initial, sizeof(input_error_initial));
		
	for( ;; )
  {
		//check for USART interrupt semaphore
		if (xSemaphoreTake(sema2_IRQ_USART_RX, portMAX_DELAY))
		{
			//read the input of the USART to myChar array
			myChars[i] = USART_ReceiveData(USART2);
			//debug3("Task_usart2_RX")
			
			//show the user what he typed
			//check if USART not in use
			if (xSemaphoreTake(sema2_msg_USART_TX, portMAX_DELAY))
			{
				while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))
				{
				}
				//send_data_on_usart only for one character
				send_data_on_usart(USART2, myChars[i]);
				if(myChars[i] == '\r')
				{
					//send also a \n
					while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))
					{
					}
					send_data_on_usart(USART2, '\n');
				}
				xSemaphoreGive(sema2_msg_USART_TX);
			}
			
			//if backspace, "delete" last character
			if(myChars[i] == '\b')
			{
				//delete backspace character
				myChars[i] = NULL;
				send_data_on_usart(USART2, ' ');
				send_data_on_usart(USART2, '\b');
				if (i > 0)
				{
					//set counter back
					i = i - 2;
				}
				else if (i == 0)
				{
					i = -1; 
				}
			}
			
			//"ENTER" RECEIVED
			//------------------------------------------------------------------------
			else if (myChars[i] == 0xD)
			{
				char char_[3];
				ID_to_string(char_, i);
				debugVar(char_);
				
				//the sent message ist to big for the buffer
				if (i >= BUFFER_SIZE-9)
				{
					send(Usart2, "(The sent message was to long, ", 31);
					send(Usart2, "it may be missing text)\n\r", 25);
				}
				
				//PC does not have own ID yet
				//----------------------------------------------------------------------
				debug2("RX2: Check if already own ID")
				my_db_entry = find_hop(db, 0);
				
				if (!my_db_entry)
				{
					debug2("Initialise new ID")
					//check if input correct
					if (myChars[0] >= '0' && myChars[0] <= '9')
					{
						//check if single digit or double digit
						if (myChars[1] == '\r')
						{
							debug2("RX2: ID is single digit")
							my_ID_int = (int)myChars[0] - '0'; //convert from char to int
							i = -1;
						}
						//check if double digit:
						else if (myChars[1] >= '0' && myChars[1] <= '9')
						{
							if (myChars[2] == '\r')
							{
								debug2("RX2: ID is double digit")
								my_ID_int = ((int)(myChars[0] - '0')) * 10 + (int)(myChars[1] - '0');
							}
							else
							{
								debug1("RX2: wrong character after double digit ID")
								send(Usart2, input_error_initial, sizeof(input_error_initial));
								i = -1;
							}
						}
						//wrong input
						else
						{
							debug1("RX2: Input wrong when initialising ID")
							my_ID_int = 0;
							send(Usart2, input_error_initial, sizeof(input_error_initial));
							i = -1;
						}
						
						//if ID between 1 and 99
						if (my_ID_int > 0 && my_ID_int < 100)
						{
							//check database if ID is available
							if (!find_pc(db, my_ID_int))
							{
								debug2("RX2: ID is free")
								id_positiv_notify[12] = (my_ID_int / 10) + '0';
								id_positiv_notify[13] = (my_ID_int % 10) + '0';
								send(Usart2, id_positiv_notify, strlen(id_positiv_notify));
								
								//update database
								//create db_UPDATE msg
								update_string[0] = (my_ID_int / 10) + '0';
								update_string[1] = (my_ID_int % 10) + '0';
								//send a update 'pc=my_pc_id' with 'hops=00' and 'interface=02' to the database
								debug2("RX2: send my ID to update my DB")
								xMessageBufferSend(msg_USART2_DB, (void *) update_string, 7, 0);
								xSemaphoreGive(sema2_DB_UPDATE);
								i = -1;
							}
							else
							{
								debug1("RX2: ID already taken!")
								send(Usart2, input_error_initial, sizeof(input_error_initial));
								my_ID_int = 0;
								i = -1;
							}
						}
						else
						{
							debug1("RX2: ID must be between 1 and 99")
							send(Usart2, input_error_initial, sizeof(input_error_initial));
							i = -1;
						}
					}
					//Input not correct
					else
					{
						debug1("RX2: Input wrong when initialising ID")
						send(Usart2, input_error_initial, sizeof(input_error_initial));
						i = -1;
					}
				}
				//PC has own ID
				//----------------------------------------------------------------------
				else
				{
					my_ID_int = my_db_entry->pc;
					
					if (strstr(myChars, "/help"))
					{
						//send help
						debug2("RX2: send help")
						send(Usart2, "\n\rTo send a message you need to first ", 39);
						send(Usart2, "define your own ID (1..99)\n\r", 29);
						send(Usart2, "Then you can send a message with:\n\r", 36);
						send(Usart2, "XX: your_message\n\r", 19);
						send(Usart2, "(XX is the ID of the receiver)\n\n\r", 34);
						send(Usart2, "other possible commands:\n\r", 28);
						send(Usart2, "/blink    -> blink the LED\n\r", 29);
						send(Usart2, "/release  -> to release ID\n\r", 29);
						send(Usart2, "/showDB   -> to show the database\n\r", 36);
						send(Usart2, "/resetDB 	-> reset the database\n\r", 34);
						send(Usart2, "/hello    -> ask neighbor for update\n\r", 39);
						send(Usart2, "/clear    -> clear terminal screen\n\r", 37);
						send(Usart2, "/credits  -> show creators\n\r", 29);
						send(Usart2, "/help     -> show this message again\n\n\r", 40);
						i = -1;
					}
					else if (strstr(myChars, "/blink"))
					{
						//release ID
						debug2("RX2: blink the LED")
						send(Usart2, "\n\033[33mBlink the LED\n\r\033[0m", 26);
						for( j = 0; j < 12; j++)
						{
							LED_green_toggle();
							vTaskDelay(pdMS_TO_TICKS(300));
						}
						i = -1;
					}
					else if (strstr(myChars, "/release"))
					{
						//release ID
						debug2("RX2: release ID")
						send(Usart2, "\nxxX Your ID was released! Xxx\n\r", 33);
						xSemaphoreGive(sema2_DB_DELETE);
						send(Usart2, input_error_initial, sizeof(input_error_initial));
						i = -1;
					}
					else if (strstr(myChars, "/showDB"))
					{
						//show database
						debug2("RX2: show database")
						send(Usart2, "\nThese are you database entries:\n\r", 35);
						send(Usart2, "XXYYZZ -> XX.. PC ID\n\r", 23);
						send(Usart2, "       -> YY.. hop count\n\r", 27);
						send(Usart2, "       -> ZZ.. connected on interface\n\r", 40);
						xSemaphoreGive(sema2_DB_REQUEST);
						i = -1;
					}
					else if (strstr(myChars, "/resetDB"))
					{
						//show database
						debug2("RX2: reset the whole database")
						xSemaphoreGive(sema1_DB_DELETE);
						xSemaphoreGive(sema6_DB_DELETE);
						xSemaphoreGive(sema2_DB_DELETE);
						send(Usart2, input_error_initial, sizeof(input_error_initial));
						i = -1;
					}
					else if (strstr(myChars, "/hello"))
					{
						debug2("RX2: send REQUEST message to neighbors")
						send(Usart2, "Sending request to neighbors\n\r", 31);
						send(Usart1, &request, sizeof(request));
						vTaskDelay(100);
						send(Usart6, &request, sizeof(request));
						i = -1;
					}
					else if (strstr(myChars, "/clear"))
					{
						debug2("RX2: clear terminal screen")
						send(Usart2, "\033[2J\033[1;1H", 11);
						i = -1;
					}
					else if (strstr(myChars, "/credits"))
					{
						debug2("RX2: show credits:")
						banner_message();
						i = -1;
					}
					
					//check if MSG
					//-----------------------------------------------------------------------
					else
					{
						//get receiver ID from message
						strtok(myChars, ":");
						
						//first character correct
						if (myChars[0] >= '0' && myChars[0] <= '9')
						{
							//check second character if number
							if (myChars[1] >= '0' && myChars[1] <= '9')
							{
								//double digit
								if (myChars[2] == '\0')
								{
									debug2("RX2: receiver ID is double digit")										
									myChars[strlen(myChars)] = ':';
									
									debug2("RX: forward formatted xx>yy: message (2)")
									//create modified message
									msg_buffer[0] = '\0';
									strcat(msg_buffer, ID_to_string(my_ID_string, my_ID_int));
									strcat(msg_buffer, ">");
									strncat(msg_buffer, myChars, i--);
									strcat(msg_buffer, "\n\r");
									
									//send char array to message buffer and notify other task
									//_debug_var("myChars = %s", myChars);
									xMessageBufferSend(msg_USART2_MSG, (void *) msg_buffer, i+6, portMAX_DELAY);
									xSemaphoreGive(sema2_MSG);
									i = -1;
								}
								//wrong char after double digit
								else
								{
									i = -1;
									debug2("RX2: ERROR: wrong character after receiver ID (2)")
									send(Usart2, input_error, strlen(input_error)+1);
								}
							}
							//[1] a number, but [2] not
							else
							{
								//single digit
								if (myChars[1] == '\0')
								{	
									debug2("RX2: receiver ID is singe digit")
									myChars[strlen(myChars)] = ':';
									
									debug2("RX: forward formatted xx>yy: message (1)")
									//create modified message
									msg_buffer[0] = '\0';
									strcat(msg_buffer, ID_to_string(my_ID_string ,my_ID_int));
									strcat(msg_buffer, ">0");
									strncat(msg_buffer, myChars, i--);
									strcat(msg_buffer, "\n\r");
									
									//send char array to message buffer and notify other task
									//_debug_var("myChars = %s", myChars);
									xMessageBufferSend(msg_USART2_MSG, (void *) msg_buffer, i+7, portMAX_DELAY);
									xSemaphoreGive(sema2_MSG);
									i = -1;
								}
								//wrong char after single digit
								else
								{
									i = -1;
									debug2("RX2: ERROR: wrong character after receiver ID (1)")
									send(Usart2, input_error, strlen(input_error)+1);
								}
							}
						}
						//first character is not a digit -> wrong input
						else
						{
							i = -1;
							debug2("RX2: ERROR: no ':' found")
							send(Usart2, input_error, strlen(input_error)+1);
						}
					}
				}
			}	
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			//end of special characters

			//increase counter until max buffer size is reached
			// -6 otherwise the would me a buffer overflow in the msg_send buffer
			if (i < (BUFFER_SIZE - 8) || i == -1)
			{
				i++;
			}
		}
  }
}

void Task_usart6_RX (void* pvParameters)
{	
	char myChars[BUFFER_SIZE];
	int i = 0;
	
	for( ;; )
  {
		//check for USART interrupt semaphore
		if (xSemaphoreTake(sema6_IRQ_USART_RX, portMAX_DELAY))
		{
			//read the input of the USART to myChar array		
			myChars[i] = USART_ReceiveData(USART6);
			//debug3("Task_usart6_RX")
			
			if (myChars[i] == '\0') i=-1;
			if (xSemaphoreTake(sema2_usart_reset,1)) i=-1;
			
			//Check char for special charakter:
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			//HELLO
			//----------------------------------------------------------------------------------
			if (myChars[i] == 0x5) {xSemaphoreGive(sema6_HELLO_ACK); i=-1;}
			
			//HELLO ACK
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x6) {xSemaphoreGive(sema6_HELLO_notify); i=-1;}
			
			//MSG
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0xD)
			{
				//send char array to message buffer and notify other task
				xMessageBufferSend(msg_USART6_MSG, (void *) myChars, ++i, 0);
				xSemaphoreGive(sema6_MSG);
				i = -1;				
			}
			
			//DB UPDATE
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x15)
			{
				//send char array to message buffer and notify other task
				xMessageBufferSend(msg_USART6_DB, (void *) myChars, ++i, 0);
				xSemaphoreGive(sema6_DB_UPDATE);
				i=-1;
			}

			//DB DELETE
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x13) {xSemaphoreGive(sema6_DB_DELETE); i=-1;}
			
			//DB REQUEST
			//----------------------------------------------------------------------------------
			else if (myChars[i] == 0x14) {xSemaphoreGive(sema6_DB_REQUEST); i=-1;}
			
			
			//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
			//end of special characters
			
			//increase counter until max buffer size is reached
			if (i < (BUFFER_SIZE - 6) || i == -1)
			{
				i++;
			}
		}
  }
}
//_________________________________________________________


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
//X																	TRANSMIT																	 X//
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX//
void Task_usart1_TX (void* pvParameters)
{
	int i;
	char receivedChars[BUFFER_SIZE];
	int size = 0;
	
	for (;;)
	{
		//wait for semaphore
		if (xSemaphoreTake(sema1_USART_TX, portMAX_DELAY))
		{
			//debug3("Task_usart1_TX")
			//receive buffer
			size = xMessageBufferReceive(msg_USART1_TX, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			if (size)
			{
				//release sema for msg_buffer => message buffer is ready again
				xSemaphoreGive(sema1_msg_USART_TX);
				//output buffer on usart
				for (i = 0; i < size; i++)
				{
					while (!get_the_flag_status_from_usart(USART1, USART_FLAG_TXE))
					{
					}
					//Delay so data gets send as a single character, not as string
					send_data_on_usart(USART1, receivedChars[i]);
					vTaskDelay(5);
				}
			}
			else
			{
				debug1("Task_usart1: received emtpy buffer!")
				xSemaphoreGive(sema1_msg_USART_TX);
			}
		}
	}
}

void Task_usart2_TX (void* pvParameters)
{
	int i, size;
	char receivedChars[BUFFER_SIZE];
	
	for (;;)
	{
		//wait for semaphore
		if (xSemaphoreTake(sema2_USART_TX, portMAX_DELAY))
		{
			//debug3("Task_usart2_TX")
			size = xMessageBufferReceive(msg_USART2_TX, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			//receive buffer
			if (size)
			{
				//release sema for msg_buffer => message buffer is ready again
				xSemaphoreGive(sema2_msg_USART_TX);
				//output buffer on usart
				for (i = 0; i < size; i++)
				{
					while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))
					{
					}
					send_data_on_usart(USART2, receivedChars[i]);
					
				}
			}
			else
			{
				debug1("Task_usart2: received emtpy buffer!")
				xSemaphoreGive(sema2_msg_USART_TX);
			}
		}
	}
}

void Task_usart6_TX (void* pvParameters)
{
	int i;
	char receivedChars[BUFFER_SIZE];
	int size = 0;
	
	for (;;)
	{
		//wait for semaphore
		if (xSemaphoreTake(sema6_USART_TX, portMAX_DELAY))
		{
			//debug3("Task_usart6_TX")
			size = xMessageBufferReceive(msg_USART6_TX, receivedChars, BUFFER_SIZE, portMAX_DELAY);
			//receive buffer
			if (size)
			{
				//release sema for msg_buffer => message buffer is ready again
				xSemaphoreGive(sema6_msg_USART_TX);
				//output buffer on usart
				for (i = 0; i < size; i++)
				{
					while (!get_the_flag_status_from_usart(USART6, USART_FLAG_TXE))
					{
					}
					//Delay so data gets send as a single character, not as string
					send_data_on_usart(USART6, receivedChars[i]);
					vTaskDelay(5);
				}
			}
			else
			{
				debug1("Task_usart6: received emtpy buffer!")
				xSemaphoreGive(sema6_msg_USART_TX);
			}
		}
	}
}
