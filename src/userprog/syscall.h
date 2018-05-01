#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void parse_args(void *esp, void **argv, int n);
void validate_pointer(const void *p);
int sys_write(int fd, const void *buffer, unsigned size);
void sys_exit(int status);

#endif /* userprog/syscall.h */