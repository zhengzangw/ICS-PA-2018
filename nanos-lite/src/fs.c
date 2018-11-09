#include "fs.h"
#include "proc.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
	size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
		Log("open");
		int fd = -1;
		for (int i=0; i<NR_FILES; ++i){
				if (strcmp(file_table[i].name, pathname)==0){
						fd = i;
						break;
				}
		}
		if (fd==-1) panic("fd == -1: NO SUCH FILE!");
		else return fd;
}

int fs_close(int fd){
		return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
		ramdisk_read(buf, file_table[fd].disk_offset, len);
		return 0;
}
size_t fs_write(int fd, const void *buf, size_t len){
		TODO();
		return 0;
}

size_t fs_lseek(int fd, size_t offset, int whence){
		TODO();
		return 0;
}

size_t fs_filesz(int fd){
		return file_table[fd].size;
}
