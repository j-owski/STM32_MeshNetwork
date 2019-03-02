// main.c

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hardware.h"
#include "sema_msg.h"
#include "task_init.h"
#include "task_database.h"

// Task priorities: Higher numbers are higher priority.
#define mainTestTask_Priority (tskIDLE_PRIORITY + 1)

void vTaskDelay(portTickType xTicksToDelay);
debug_var
debug_counter

char debug_int;

int main(void)
{
	database *db;
	//init Hardware
	init_hardware();	
	//init Database
	db = init_database();
	//init Semaphores
	sema_INIT();
	//init message buffers
	msg_INIT();
	//create tasks
	task_INIT(db);
	
	//start the scheduler
  vTaskStartScheduler();
	
  for( ;; );  
}


//convert a ID int value into a three byte char buffer (2 + 1 for the terminating NULL char)
//pass the char array in which the converted value ID value is saved in
char* ID_to_string(char* save_to_char_buffer, int IDvalue)
{
	save_to_char_buffer[0] = (char)(IDvalue/10 + '0');
	save_to_char_buffer[1] = (char)(IDvalue%10 + '0');
	save_to_char_buffer[2] = NULL;
	
	return save_to_char_buffer;
}

//send given data out on USART X
void send(int USARTx, char* message, int size_of_message)
{
	//debug4("send")
	//if usart 1 was selected
	if (USARTx == 1)
	{
		if (xSemaphoreTake(sema1_msg_USART_TX, portMAX_DELAY))
		{
			xMessageBufferSend(msg_USART1_TX, message, size_of_message , portMAX_DELAY);
			xSemaphoreGive(sema1_USART_TX);
		}
	}
	//usart 2
	else if (USARTx == 2)
	{
		if (xSemaphoreTake(sema2_msg_USART_TX, portMAX_DELAY))
		{
			xMessageBufferSend(msg_USART2_TX, message, size_of_message, portMAX_DELAY);
			xSemaphoreGive(sema2_USART_TX);
		}
	}
	//usart 6
	else if (USARTx == 6)
	{
		if (xSemaphoreTake(sema6_msg_USART_TX, portMAX_DELAY))
		{
			xMessageBufferSend(msg_USART6_TX, message, size_of_message, portMAX_DELAY);
			xSemaphoreGive(sema6_USART_TX);
		}
	}
}

void banner_message(void)
{
	send(Usart2, "\x1b[36m\n\r __  ___  __   __   ", 28);
	send(Usart2, " __                    \n\r", 26);
	send(Usart2, "|__)  |  /  \\ (_    ", 21);
	send(Usart2, "|__)  _  _  .  _ |  |_ \n\r", 26);
	send(Usart2, "| \\   |  \\__/ __)   ", 21);
	send(Usart2, "|    |  (_) | (- |( |_ \n\r", 26);
	send(Usart2, "                    ", 21);
	send(Usart2, "            /          \n\n\r", 27);
	send(Usart2, "by:  ~ Arseven Can\n\r", 21);
	send(Usart2, "     ~ Abo Galila Mohamed\n\r", 28);
	send(Usart2, "     ~ Ostrowski John\n\n\r\x1b[0m", 29);
}

void debug_error_print ()
{
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'E');		
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'R');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'R');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'O');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'R');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ':');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');		
	for (int i = 0; i < strlen(debug_buff); i++)	{
		while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {} send_data_on_usart(USART2, debug_buff[i]); }
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\r');
}

void debug_msg_print ()
{	
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'D');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'E');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'B');		
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'U');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'G');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ':');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');		
	for (int i = 0; i < strlen(debug_buff); i++)	{
		while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {} send_data_on_usart(USART2, debug_buff[i]); }
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\r');
}

void debug_var_print ()
{	
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '>');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'v');		
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'a');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, 'r');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ':');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');		
	for (int i = 0; i < strlen(debug_buff); i++)	{
		while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {} send_data_on_usart(USART2, debug_buff[i]); }
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\r');
}

void debug_task_print ()
{
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '-');
	
	for (int i = 0; i < strlen(debug_buff); i++)	{
		while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {} send_data_on_usart(USART2, debug_buff[i]); }
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{} send_data_on_usart(USART2, '\r'); 
}

void debug_func_print ()
{
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ' ');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, 'f');		
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, 'u');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, 'n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, 'c');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ':');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, ' ');
	for (int i = 0; i < strlen(debug_buff); i++)	{
		while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE)) {}; send_data_on_usart(USART2, debug_buff[i]); }
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, '\n');
	while (!get_the_flag_status_from_usart(USART2, USART_FLAG_TXE))	{}; send_data_on_usart(USART2, '\r'); 
}
