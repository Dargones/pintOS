#include "userprog/syscall.h"
#include "devices/shutdown.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	switch(f->esp){
	case(0):
		s_halt();
		break;
	case(1):
		s_exit(status);
		break;
	case(2):
		s_exec();
		break;
	case(3):
		s_wait();
		break;
	case(4):
		s_create();
		break;
	case(5):
		s_remove();
		break;
	case(6):
		s_open();
		break;
	case(7):
		s_open();
		break;
	case(8):
		s_filesize();
		break;
	case(9):
		s_read();
		break;
	case(10):
		s_write();
		break;
	case(11):
		s_seek();
		break;
	case(12):
		s_tell();
		break;
	case(13):
		s_close();
		break;
	}
  printf ("system call!\n");
  thread_exit ();
}

void
s_halt(void)
{
shutdown_power_off();
}

bool
s_create(const char* file, unsigned initial_size)
{
	return filesys_create (file, initial_size) ;
}



