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
		int fd = -1;
		for (int i=0; i<NR_FILES; ++i){
				if (strcmp(file_table[i].name, pathname)==0){
						fd = i;
						break;
				}
		}
		Log("%d", fd);
		if (fd==-1) panic("No Such File!");
		else return fd;
}

int fs_close(int fd){
		return 0;
}

ssize_t fs_read(int fd, void *buf, size_t len){
		Log("read");
		size_t left = file_table[fd].disk_offset + file_table[fd].size - file_table[fd].open_offset;
		size_t real_len = len < left ? len : left;
		ramdisk_read(buf, file_table[fd].disk_offset+file_table[fd].open_offset, real_len);
		file_table[fd].open_offset += real_len;
		return real_len;
}

ssize_t fs_write(int fd, const void *buf, size_t len){
		if (len > file_table[fd].size) panic("Write into file exceeding its oringinal size");
		
		ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
		return len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
		off_t start;
		switch (whence) {
				case SEEK_SET: start = file_table[fd].disk_offset; break;
				case SEEK_CUR: start = file_table[fd].open_offset; break;
				case SEEK_END: start = file_table[fd].disk_offset + file_table[fd].size; break;
				default: panic("fs_lseek: whence Invalid!");
		}

		off_t pos = start + offset;
		if (file_table[fd].disk_offset<=pos && pos <= file_table[fd].disk_offset + file_table[fd].size){
      file_table[fd].open_offset = pos;
		} else {
		  panic("Out of file bound!");
			return -1;
		}

		return pos - file_table[fd].disk_offset;
}

size_t fs_filesz(int fd){
		return file_table[fd].size;
}
