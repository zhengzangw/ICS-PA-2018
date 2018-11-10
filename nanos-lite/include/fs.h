#ifndef __FS_H__
#define __FS_H__

#include "common.h"
#include <unistd.h>
#include <sys/stat.h>

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

int fs_open(const char *, int, int);
ssize_t fs_read(int, void *, size_t);
ssize_t fs_write(int, const void *, size_t);
off_t fs_lseek(int, off_t, int);
int fs_close(int);
size_t fs_filesz(int);
size_t serial_write(const void *, size_t, size_t);
size_t fb_write(const void *, size_t, size_t);
size_t dispinfo_read(void *, size_t, size_t);

#endif
