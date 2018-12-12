#ifndef __PROC_H__
#define __PROC_H__

#include "common.h"
#include "memory.h"

#define STACK_SIZE (8 * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    _Context *cp;
    _Protect as;
    uintptr_t cur_brk;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;

void naive_uload(PCB *, const char *);
void args_uload(PCB *, const char *, char* argv[], char *envp[]);
size_t ramdisk_write(const void *, size_t, size_t);
size_t ramdisk_read(void *, size_t, size_t);
size_t get_ramdisk_size();
_Context* schedule(_Context*);
void context_kload(PCB *, void *);
void context_uload(PCB *pcb, const char *filename);
int mm_brk(uintptr_t);

#endif
