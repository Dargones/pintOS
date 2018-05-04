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
#include "threads/palloc.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);
struct file_descriptor *find_file_desc(struct thread *t, int id);
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
	validate_pointer(f->esp);
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
      		f->eax = process_wait((tid_t)argv[0]);
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
    	case SYS_OPEN: {
      		get_args(f->esp, argv, 1);
      		f->eax = sys_open((char *)argv[0]);
      		break;
   		}
   		case SYS_CLOSE: {
   			get_args(f->esp, argv, 1);
   			sys_close((int)argv[0]);
      		break;
   		}
	    case SYS_FILESIZE: {
	      	get_args(f->esp, argv, 2);
	      	f->eax = sys_filesize((int)argv[0]);
	     	break;
	    }
	    case SYS_READ: {
		    get_args(f->esp, argv, 3);
		    f->eax = sys_read((int)argv[0], argv[1], (unsigned)argv[2]);
		    break;
	    }
	}
}

void validate_pointer(void *p) {
	if ((p==NULL) || (p < CODE_AREA_STARTS) || (p >= PHYS_BASE)) 
		sys_exit(-1);
}

void sys_exit(int status) {
  printf("%s: exit(%d)\n", thread_current()->name, status);
  struct child_info *info = thread_current()->info;
  if(info != NULL) {
    info->exitcode = status;
    ASSERT(status!=RUNNING);
    sema_up(&(info->sema));
  }
  thread_exit();
}

int sys_write(int id, void *buffer, unsigned size) {
  int result;	
  validate_pointer(buffer);
  validate_pointer(buffer + size -1);
  lock_acquire (&sys_lock);
  if(id == 1) { 
    putbuf(buffer, size);
    lock_release (&sys_lock);
    return size;
  }
  struct file_descriptor* fd = find_file_desc(thread_current(), id);
  if(fd && fd->file)  {
  	result = file_write(fd->file, buffer, size);
    lock_release (&sys_lock);
    return result;
  }
  lock_release (&sys_lock);
  return -1;
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
  //if (filename == "")
  	//return -1; /* for create_empty test*/
  bool return_code;
  lock_acquire (&sys_lock); 
  return_code = filesys_create(filename, initial_size);
  lock_release (&sys_lock);
  return return_code;
}

int sys_open(char* file_name) {
  validate_pointer(file_name);
  struct file *f;
  struct file_descriptor *fd = palloc_get_page(0);
  if (!fd)
    return -1;
  lock_acquire (&sys_lock);
  f = filesys_open(file_name);
  if (!f) {
    palloc_free_page (fd);
    lock_release (&sys_lock);
    return -1;
  }
  struct list* files = &thread_current()->files_list;
  if (list_empty(files))
    fd->id = 3;
  else
    fd->id = (list_entry(list_back(files), struct file_descriptor, elem)->id) + 1;
  fd->file = f;
  list_push_back(files, &(fd->elem));
  lock_release (&sys_lock);
  return fd->id;
}

void sys_close(int id) {
  lock_acquire (&sys_lock);
  struct file_descriptor *fd = find_file_desc(thread_current(), id);
  if (fd) {
    file_close(fd->file);
    list_remove(&(fd->elem));
    palloc_free_page(fd);
  }
  lock_release (&sys_lock);
}

struct file_descriptor *find_file_desc(struct thread *t, int id) {
  if (id < 3)
    return NULL;
  struct list_elem *e;
  if (! list_empty(&t->files_list)) {
    for(e = list_begin(&t->files_list); e != list_end(&t->files_list); e = list_next(e)) {
      struct file_descriptor *fd = list_entry(e, struct file_descriptor, elem);
      if(fd->id == id)
        return fd;
    }
  }
  return NULL; 
}

int sys_read(int id, void *buffer, unsigned size) {
  int result, i;
  validate_pointer(buffer);
  validate_pointer(buffer + size -1);
  lock_acquire (&sys_lock);

  if(id == 0) { 
    /* TODO */
    lock_release (&sys_lock);
    return -1;
  }
  struct file_descriptor* fd = find_file_desc(thread_current(), id);
  if(fd && fd->file) {
    result = file_read(fd->file, buffer, size);
    lock_release (&sys_lock);
    return result;
  }
  lock_release (&sys_lock);
  return -1;
}

struct file* process_get_file(int fd){

}

int sys_filesize(int id){
	lock_acquire(&sys_lock);
	struct file* f = find_file_desc(thread_current(), id)->file;

	if(!f){
		lock_release(&sys_lock);
		return -1;
	}

	int filesize = file_length(f);
	lock_release(&sys_lock);
	return filesize;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}

void get_args(void **esp, void **argv, int argc) {
	int i;
	for (i = 0; i< argc; i++) {
		validate_pointer(esp + i + 1);
		argv[i] = *(esp + i + 1);
	}
}