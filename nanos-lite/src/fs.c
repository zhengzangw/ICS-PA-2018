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
#define VFS_NUM 7
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[3].size = screen_width()*screen_height()*32;

	Log("fb_size = %u", file_table[3].size);
}

int fs_open(const char *pathname, int flags, int mode){
	int fd = -1;
	for (int i=0; i<NR_FILES; ++i){
		if (strcmp(file_table[i].name, pathname)==0){
		fd = i;
		break;
		}
	}
	if (fd==-1) panic("File %s not exist", pathname);
    else {
        file_table[fd].open_offset = 0;
        Log("File %s fd=%d loaded", pathname, fd);
        return fd;
    }
}

int fs_close(int fd){
    file_table[fd].open_offset = 0;
	return 0;
}


ssize_t fs_read(int fd, void *buf, size_t len){
    size_t inclen=0;

	if (file_table[fd].read){
	    inclen = file_table[fd].read(buf, file_table[fd].open_offset, len);
	} else {
	  size_t left = file_table[fd].size - file_table[fd].open_offset;
	  inclen = len < left ? len : left;
	  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, inclen);
	}

	file_table[fd].open_offset += inclen;
	return inclen;
}

ssize_t fs_write(int fd, const void *buf, size_t len){
		if (file_table[fd].write!=NULL){
			size_t real_len = file_table[fd].write(buf, file_table[fd].open_offset, len);
			file_table[fd].open_offset += real_len;
			return real_len;
		} else {
		  size_t left = file_table[fd].size - file_table[fd].open_offset;
		  if (left<len) panic("Out of File Boundary!");
		  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		  file_table[fd].open_offset += len;
		  return len;
		}
}

off_t fs_lseek(int fd, off_t offset, int whence){
		off_t start;
		switch (whence) {
				case SEEK_SET: start = file_table[fd].disk_offset; break;
				case SEEK_CUR: start = file_table[fd].disk_offset + file_table[fd].open_offset; break;
				case SEEK_END: start = file_table[fd].disk_offset + file_table[fd].size; break;
				default: panic("fs_lseek: whence Invalid!");
		}

		off_t pos = start + offset;
		if (fd<VFS_NUM || (file_table[fd].disk_offset<=pos && pos <= file_table[fd].disk_offset + file_table[fd].size)){
            file_table[fd].open_offset = pos - file_table[fd].disk_offset;
		} else {
		  panic("End of File is %u, pointer locates %u. Out of file bound!", file_table[fd].disk_offset+file_table[fd].size, pos);
		  return -1;
		}

		return file_table[fd].open_offset;
}

size_t fs_filesz(int fd){
	return file_table[fd].size;
}
