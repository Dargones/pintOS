#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#define CODE_AREA_STARTS 0x08048000
#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#define FIRST_FILE_ID 3

void syscall_init (void);
void parse_args(void *esp, void **argv, int n);
void validate_pointer(void *p);
int sys_write(int fd, void *buffer, int size);
int sys_wait(int pid);
void sys_exit(int status);
int sys_create(char* filename, int initial_size);
void get_args(void **esp, void **argv, int argc);
int sys_exec(char *cmdline);
int sys_open(char* file);
void sys_close(int id);
int sys_read(int id, void *buffer, int size);
int sys_filesize(int id);


#endif /* userprog/syscall.h */