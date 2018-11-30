#include "proc.h"
#include "fs.h"

#define DEFAULT_ENTRY 0x4000000
#define FILE_SYSTEM
uint8_t buffer[0x3000000];

static uintptr_t loader(PCB *pcb, const char *filename) {
	size_t size = get_ramdisk_size();
	Log("Size of ramdisk = %u", size);

#ifdef FILE_SYSTEM
	int fd = fs_open(filename, 0, 0);
	size_t fs_size = fs_filesz(fd);
	fs_read(fd, buffer, fs_size);
	memcpy((uintptr_t *)DEFAULT_ENTRY, buffer, fs_size);
	Log("File %s loaded",filename);
#else
	ramdisk_read(buffer, 0, size);
	memcpy((uintptr_t *)DEFAULT_ENTRY, buffer, size);
	Log("Ramdisk initialized!");
#endif

  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
