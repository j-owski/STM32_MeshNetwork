//
// Created by Can on 18.11.2018.
//
#include "task_database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "sema_msg.h"
#include "hardware.h"

// pc .... Computer (PC) identifier number
// hop ... hop count
// int ... interface number (can be usart 1, 2 or 6 -> STM32F401re)
//
// database structure: [int pc, int hop, int interface]   
//
//      O  [1, 1, 6] ... PC1 on interface 6 with 1 hop
//     /
//    /
//   O  [1, 0, 2] ... PC1 on interface 2 with 0 hops
//   |
//   PC1

//initializes the database
//returns the pointer to that database
database* init_database(void)
{
	database *db;
  db = pvPortMalloc(sizeof(db));
  db->count = 0;
  db->head = NULL;
  db->tail = NULL;
	
	return db;
}

//inserts new data at the end of the given database
int insertEntry(database *db, int pc, int hop, int interface)
{
	char error_malloc[] = "No memory for new database-entry\n\r\0";
	char new_entry[] = "\x1b[35mPCxx connected!\n\r\x1b[0m\0";
		
	if (xSemaphoreTake(sema_db, portMAX_DELAY))
	{
		debug3("DB_insertEntry")
		
		db_data *newData = pvPortMalloc(sizeof(db_data));
		if (!newData)
		{
			debug1("@@@@@@@@@@ malloc failed!")
			send(Usart2, error_malloc, strlen(error_malloc));
			xSemaphoreGive(sema_db);
			xSemaphoreGive(sema2_usart_reset);
			taskYIELD();
			return 0;
		}
		newData->pc = pc;
		newData->hop = hop;
		newData->interface = interface;
		newData->next = NULL;
		db->count++;
		
		//empty list
		if (db->head == NULL)
		{
			newData->previous = NULL;
			db->head = newData;
			db->tail = newData;
			debug2("DB: entry added (first entry in db)")
			xSemaphoreGive(sema_db);
			
			//inform the PC of new connection
			new_entry[7] = (pc / 10) + '0';
			new_entry[8] = (pc % 10) + '0';
			send(Usart2, "\n\r", 2);
			send(Usart2, new_entry, strlen(new_entry));
			return 1;
		}
		//list not empty
		else
		{
			newData->previous = db->tail;
			db->tail->next = newData;
			db->tail = newData;
		}
		debug2("DB: entry added")
		xSemaphoreGive(sema_db);
		
		//inform the PC of new connection
		new_entry[7] = (pc / 10) + '0';
		new_entry[8] = (pc % 10) + '0';
		send(Usart2, new_entry, strlen(new_entry));
		debug_timer
	}
	return 1;
}

//delete entry in database
void deleteEntriesForInterface(database *db, int interface)
{
	db_data *temp;
	char lost_connection[] = "Lost connection of usartx\n\r";
	
	if (xSemaphoreTake(sema_db, portMAX_DELAY))
	{
		debug3("DB_deleteEntriesForInterface")
		
		temp = db->head;
		
		//cyicle through whole database
		while(temp)
		{
			//found a interface
			if(temp->interface == interface)
			{
				db->count--;
				
				//if at the beginning of the list
				if (temp == db->head)
				{
					//if also the tail -> the only entry in db
					if (temp == db->tail)
					{
						db->head = NULL;
						db->tail = NULL;
					}
					//not tail
					else 
					{
						db->head = temp->next;
						temp->next->previous = NULL;
					}
				}
				//not at the beginning
				else
				{
					//and also the tail
					if (temp == db->tail)
					{
						db->tail = temp->previous;
						temp->previous->next = NULL;
					}
					//and not the tail -> in the middle
					else
					{
						temp->previous->next = temp->next;
						temp->next->previous = temp->previous;
					}
				}
				//free up space
				vPortFree(temp);
			}
			temp = temp->next;
		}
		xSemaphoreGive(sema_db);
		
		lost_connection[24] = (interface % 10) + '0';
		send(Usart2, lost_connection, sizeof(lost_connection));
		debug2("DB: entry deleted ")
	}
}

// search database for first entry by PC identifier number; returns the entry ptr
db_data* find_pc(database *db, int pc)
{
	if (xSemaphoreTake(sema_db, portMAX_DELAY))
	{
		db_data *temp = db->head;
		while (temp != NULL)
		{
			if (temp->pc == pc)
			{
				xSemaphoreGive(sema_db);
				return temp;
			}
			else
			{
			temp = temp->next;
			}
		}
		xSemaphoreGive(sema_db);
	}
	return NULL;
}

// search database for first entry by hop
db_data* find_hop(database *db, int hop)
{
	if (xSemaphoreTake(sema_db, portMAX_DELAY))
	{
		db_data *temp = db->head;
		while (temp != NULL)
		{
			if (temp->hop == hop)
			{
				//debug4("DB_find_hop: Found")
				xSemaphoreGive(sema_db);
				return temp;
			}
			else
			{
			temp = temp->next;
			}
		}
		debug4("DB_find_hop: not found")
		xSemaphoreGive(sema_db);
		return NULL;
	}
	return NULL;
}

// search database for first entry by interface
db_data* find_interface(database *db, int inter)
{
	if (xSemaphoreTake(sema_db, portMAX_DELAY))
	{
		db_data *temp = db->head;
		while (temp != NULL)
		{
			if (temp->interface == inter)
			{
				xSemaphoreGive(sema_db);
				return temp;
			}
			else
			{
			temp = temp->next;
			}
		}
		//debug4("DB_find_interface: not found")
		xSemaphoreGive(sema_db);
	}		
	return NULL;
}
