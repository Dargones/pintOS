#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#define CODE_AREA_STARTS 0x08048000
void syscall_init (void);
void parse_args(void *esp, void **argv, int n);
void validate_pointer(void *p);
int sys_write(int fd, void *buffer, unsigned size);
int sys_wait(int pid);
void sys_exit(int status);
int sys_create(char* filename, unsigned initial_size);
void get_args(void **esp, void **argv, int argc);
int sys_exec(char *cmdline);

#endif /* userprog/syscall.h */