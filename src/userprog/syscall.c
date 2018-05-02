#include "userprog/syscall.h"
#include "devices/shutdown.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *);
struct lock sys_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&sys_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{	
	void *argv[MAX_ARGS];
	int call_num = *(int *)f->esp;
	//printf("callnum: %d\n", call_num);

	switch(call_num) {
		case SYS_HALT: {
			shutdown_power_off();
			break;
		} case SYS_EXIT: {
			get_args(f->esp, argv, 1);
      		sys_exit((int)argv[0]);
      		break;
		} case SYS_WRITE: {
			get_args(f->esp, argv, 3);
      		f->eax = sys_write((int)argv[0], argv[1], (unsigned)argv[2]);
      		break;
	    }
		case SYS_WAIT: {
			get_args(f->esp, argv, 1);
      		f->eax = sys_wait((tid_t)argv[0]);
      		break;
    	}
    	case SYS_EXEC: {
    		get_args(f->esp, argv, 1);
      		f->eax = sys_exec((char*) argv[0]);
      		break;
    	}
    	case SYS_CREATE: {
    		get_args(f->esp, argv, 2);
    		f->eax = sys_create((char*) argv[0], (int) argv[1]);
    	}
	}
}

void validate_pointer(void *p) {
	if ((p < CODE_AREA_STARTS) || (p >= PHYS_BASE)) {
		sys_exit(-1);
	}
}

void sys_exit(int status) {
  printf("%s: exit(%d)\n", thread_current()->name, status);
  struct child_info *info = thread_current()->info;
  //printf("child info acquired\n");
  if(info != NULL) {
  	//printf("child info is not NULL\n");
    info->exitcode = status;
    //printf("Exitcode_set\n");
    ASSERT(status!=RUNNING);
    //printf("Asserion done\n");
    //printf("id: %d\n", info->tid);
    //printf("sema_value: %d\n", info->sema.value);
    sema_up(&(info->sema));
    //printf("child info updated. Calling thread_exit()\n");
  }
  thread_exit();
  //printf("Thread_exited\n");
}

int sys_write(int fd, void *buffer, unsigned size) {
  validate_pointer(buffer);
  if(fd == 1) { 
    putbuf(buffer, size);
    return size;
  }
  return 0;
}

int sys_wait(int pid) {
  return process_wait(pid);
}

tid_t sys_exec(char *cmdline) {
  validate_pointer(cmdline);
  lock_acquire (&sys_lock); 
  tid_t pid = process_execute(cmdline);
  lock_release (&sys_lock);
  return pid;
}

int sys_create(char* filename, unsigned initial_size) {
  validate_pointer(filename);
  bool return_code;
  lock_acquire (&sys_lock); 
  return_code = filesys_create(filename, initial_size);
  lock_release (&sys_lock);
  return return_code;
}

void get_args(void **esp, void **argv, int argc) {
	int i;
	for (i = 0; i< argc; i++) {
		validate_pointer(esp + i + 1);
		argv[i] = *(esp + i + 1);
	}
}