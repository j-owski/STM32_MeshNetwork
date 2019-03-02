// main.h
#ifndef __MAIN_H
#define __MAIN_H

#define Usart1 1
#define Usart2 2
#define Usart6 6

//if you set DEBUG higher than 0, you might need a Keil Pro license
#define DEBUG 0

extern char debug_int;

extern char debug_buff[50];
extern int debug_count;
void debug_error_print(void);
void debug_msg_print(void);
void debug_var_print(void);
void debug_task_print(void);
void debug_func_print(void);
void banner_message(void);

//DEBUG MACROS
//--------------------------------------------------------------------------
#ifndef __stdio_h
#include <stdio.h>
#endif 

#ifndef __string_h
#include <string.h>
#endif

#ifndef HARDWARE_H
#include "hardware.h"
#endif

#define debug_var char debug_buff[50];
#define debug_counter int debug_count;
											
//For Debug MSG
#define debug1(string) if (DEBUG >= 1) { strncpy(debug_buff,string,50); debug_error_print(); }
#define debug2(string) if (DEBUG >= 2) { strncpy(debug_buff,string,50); debug_msg_print(); }
#define debugVar(string) if (DEBUG >= 2) { strncpy(debug_buff,string,50); debug_var_print(); }

//For debugging Tasks
#define debug3(string) if (DEBUG >= 3) {	strncpy(debug_buff,string,50); debug_task_print(); }
#define debug4(string) if (DEBUG == 4) {	strncpy(debug_buff,string,50); debug_func_print(); }

//if normal messages interfere with debug messages -> insert this before the debug msg
#define debug_timer if (DEBUG > 0) { for( debug_count = 0; debug_count < 150000; debug_count++); }

//--------------------------------------------------------------------------

char* ID_to_string (char* save_to_char_buffer, int IDvalue);
void send(int USARTx, char* message, int size_of_message);


#endif /* __MAIN_H */
