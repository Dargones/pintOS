#include "userprog/syscall.h"
#include "devices/shutdown.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
static int get_argument (void *src, void *dst, size_t bytes);
static int32_t get_byte (const uint8_t *uaddr);
int sys_wait(int pid);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
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
			//printf("exiting\n");
			int exitcode;
      		get_argument(f->esp + 4, &exitcode, sizeof(exitcode));
      		sys_exit(exitcode);
      		break;
		} case SYS_WRITE: {
	      	int fd, return_code;
      		const void *buffer;
      		unsigned size;

      		get_argument(f->esp + 4, &fd, sizeof(fd));
      		get_argument(f->esp + 8, &buffer, sizeof(buffer));
      		get_argument(f->esp + 12, &size, sizeof(size));

      		return_code = sys_write(fd, buffer, size);
      		f->eax = (uint32_t) return_code;
      		break;
	    }
		case SYS_WAIT: {
			//printf("attempting to wait\n");
      		int pid;
      		get_argument(f->esp + 4, &pid, sizeof(int));
      		int ret = sys_wait(pid);
      		f->eax = (uint32_t) ret;
      		break;
    	}
	}
}

void validate_pointer(const void *p) {
	if ((p == NULL) || (p >= PHYS_BASE)) {
		/* TODO */
		return;
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

int sys_write(int fd, const void *buffer, unsigned size) {
  if(fd == 1) { // write to stdout
    putbuf(buffer, size);
    return size;
  }
  return 0;
}

int sys_wait(int pid) {
  return process_wait(pid);
}