#ifndef _GCOV_SYSCALL_H
#define _GCOV_SYSCALL_H

typedef void (*gcov_dump_fn)(const char *buf, int len);

int _gcov_open_file(const char *name, int flags, int mode);
void _gcov_end(void);
int _gcov_write_file(int file, char *ptr, int len);
void _gcov_set_dump_fn(gcov_dump_fn fn);

#endif // _GCOV_SYSCALL_H
