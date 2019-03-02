// task_database.h

#ifndef TASK_DATABASE_H
#define TASK_DATABASE_H

typedef struct
{
  int count;
  struct data *head;
  struct data *tail;
} database;


typedef struct data
{
	struct data *next;
  struct data *previous;
  int pc;
  int hop;
  int interface;
} db_data;

database* init_database(void);
int insertEntry(database *db, int pc, int hop, int interface);
void deleteEntriesForInterface(database *db, int interface);
db_data* find_pc(database *db, int pc);
int find_newID(database *db, int pc);
db_data* find_hop(database *db, int hop);
db_data* find_interface(database *db, int inter);

void send_intMetric(int UsartX, int pc, int hop, int interface);

#endif
