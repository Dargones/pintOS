#include <stdio.h>
#include <syscall-nr.h>
#include <string.h>
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *); 
struct file_descriptor *get_fd(int id); /* moving the declaration here because
it requires filesys/file.h being imported*/
struct lock lock; /* the lock that governs critical areas in write, 
create and other system calls*/

/*
Initializes the system call handler (was already present) 
*/
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&lock); /* initialize the lock*/
}

static void syscall_handler (struct intr_frame *f UNUSED) {	
	void *argv[MAX_ARGS]; /* an array where all the arguments will be stored */
	validate_pointer(f->esp); /* making sure that esp is a valid pointer */
	int call_num = *(int *)f->esp;
	switch(call_num) {
		case SYS_HALT: {
			shutdown_power_off() ;
			break;
		} case SYS_EXIT: {
			get_args(f->esp, argv, 1);
      		sys_exit((int)argv[0]);
      		break;
		} case SYS_WRITE: {
			get_args(f->esp, argv, 3);
      		f->eax = sys_write((int)argv[0], argv[1], (int)argv[2]);
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
		    f->eax = sys_read((int)argv[0], argv[1], (int)argv[2]);
		    break;
	    }
	}
}

/*
Record the exitcode STATUS in the child_info structure and exit.
*/
void sys_exit(int status) {
  /* This print statement is needed to pass the tests */
  printf("%s: exit(%d)\n", thread_current()->name, status);
  struct child_info *info = thread_current()->info;
  info->exitcode = status;
  ASSERT(status!=RUNNING); /* Because RUNNING was set to an arbitrary value,
  we use this assert statement to make sure that no real exitode equals
  RUNNING*/
  sema_up(&(info->sema)); /* Release the waiting parent*/
  thread_exit();
}

/*
Write BUFFER that has size SIZE to the file indexed as ID,.
*/
int sys_write(int id, void *buffer, int size) {
  int result;	
  validate_pointer(buffer); /* the ebeginning of the string to print */
  validate_pointer(buffer + size -1); /* the end of the string to print */
  lock_acquire (&lock);
  if(id == STD_OUT) { 
    putbuf(buffer, size);
    lock_release (&lock);
    return size;
  }
  struct file_descriptor* fd = get_fd(id); /* load the file_descripor
  that has this id*/
  if (fd != NULL) {
  	result = file_write(fd->file, buffer, size); 
  	/* the number of bytes actually written (result) may depend from that
  	requested */
    lock_release (&lock);
    return result;
  }
  lock_release (&lock);
  return -1;
}

/*
Execute a file named FILE_NAME 
*/
tid_t sys_exec(char *file_name) {
  validate_pointer(file_name);
  lock_acquire (&lock); 
  tid_t result = process_execute(file_name);
  lock_release (&lock);
  return result;
}

/*
Create a file named FILE_NAME wit hinitial size being SIZE
*/
int sys_create(char* file_name, int size) {
  validate_pointer(file_name);
  char *ch = file_name;
  bool only_spaces = true;
  /* TODO */
  while (ch != NULL) 
  	if (*(ch++) != DELIM)
  		only_spaces = false;
  if (only_spaces)
  	return false;
  lock_acquire (&lock); 
  bool result = filesys_create(file_name, size);
  lock_release (&lock);
  return result;
}

/*
Opens a file called FILE_NAME. Creates a file_descriptor unique for this
thread and pushes it on to the list of files this thread has opeed
*/
int sys_open(char* file_name) {
  validate_pointer(file_name);
  struct file *f;
  struct file_descriptor *fd = palloc_get_page(0);
  if (fd == NULL)
    return -1; /* failed to allocate memory*/
  lock_acquire (&lock);
  f = filesys_open(file_name);
  if (f == NULL) { /* if such file does not exist*/
    palloc_free_page (fd);
    lock_release (&lock);
    return -1;
  }
  struct list* files = &thread_current()->files_list;
  if (list_empty(files))
    fd->id = FIRST_FILE_ID;
  else {
  	/* the last file descriptor on the list always has the largest
  	id currently in use because it is the last file_descriptor pushed 
  	on the list */
    fd->id = (list_entry(list_back(files), 
    	struct file_descriptor, elem)->id) + 1;
  }
  fd->file = f;
  list_push_back(files, &(fd->elem));
  lock_release (&lock);
  return fd->id;
}


/* 
Close the file that is indexed as ID and free the page which was allocated 
for the corresponding file descriptor
*/
void sys_close(int id) {
  lock_acquire (&lock);
  struct file_descriptor *fd = get_fd(id);
  if (fd != NULL) {
    file_close(fd->file);
    list_remove(&(fd->elem));
    palloc_free_page(fd);
  }
  lock_release (&lock);
}

/* 
This is very similar to the write system call. 
Read SIZE bytes from FILE into BUFFER.
*/
int sys_read(int id, void *buffer, int size) {
  int result, i;
  validate_pointer(buffer);
  validate_pointer(buffer + size -1);
  lock_acquire (&lock);
  if(id == STD_IN) { 
    /* TODO */
    lock_release (&lock);
    return -1;
  }
  struct file_descriptor* fd = get_fd(id);
  if (fd != NULL) {
    result = file_read(fd->file, buffer, size);
    lock_release (&lock);
    return result;
  }
  lock_release (&lock);
  return -1;
}

/* 
Return the size of the file indexed with ID. Return -1 if no
such file is opened
*/
int sys_filesize(int id) {
	lock_acquire(&lock);
	struct file_descriptor* fd = get_fd(id);
	if(fd != NULL){
		int filesize = file_length(fd->file);
		lock_release(&lock);
		return filesize;
	}
	lock_release(&lock);
	return -1;
}

/* 
Get ARGC arguments from the stack ESP and record them inside ARGV
*/
void get_args(void **esp, void **argv, int argc) {
	int i;
	for (i = 0; i < argc; i++) {
		validate_pointer(esp + i + 1); /* the first argument is the 
		system call number and it is already validated */
		argv[i] = *(esp + i + 1);
	}
}

/* 
Search for the file descriptor indexed with ID inside the list of
file descriptors that the current thread holds
*/
struct file_descriptor *get_fd(int id) {
  struct thread *t = thread_current();
  if ((id == STD_IN) || (id == STD_OUT) || (id == STD_ERR))
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

/* 
Validates the pointer so that it can be safely dereferenced
*/
void validate_pointer(void *p) {
	if ((p==NULL) || (p < CODE_AREA_STARTS) || (p >= PHYS_BASE) ||
		(pagedir_get_page(thread_current()->pagedir, p)) == NULL) {
		sys_exit(-1); /* exit if the pointer is invalid*/
	}
}
