#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#define DELIM " "
#define MAX_ARGS 128 /* the maximum number of arguments. The manual explicitely
states not to use it, but it is a good estmation an an upper bound*/

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
