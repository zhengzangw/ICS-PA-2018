#include "proc.h"
#include "fs.h"

#define DEFAULT_ENTRY 0x4000000
#define FILE_SYSTEM

static uintptr_t loader(PCB *pcb, const char *filename) {
#ifdef FILE_SYSTEM
	int fd = fs_open(filename, 0, 0);
	fs_read(fd, (void *)DEFAULT_ENTRY, fs_filesz(fd));
#else
	ramdisk_read((void *)DEFAULT_ENTRY, 0, get_ramdisk_size());
	Log("Ramdisk initialized!");
#endif

  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}


void args_uload(PCB *pcb, const char *filename, char *argv[], char *envp[]){
    Log("\n\n\nHERE\n\n\n");
    char sargv[16][32];
    char senvp[16][32];
    char *pargv[16];
    char *penvp[16];

    int i = 0;
    while (argv[i]){
        strcpy(sargv[i], argv[i]);
        pargv[i] = sargv[i];
        i++;
    }
    int argc = i;

    i = 0;
    while (envp[i]){
        strcpy(senvp[i], envp[i]);
        penvp[i] = senvp[i];
        i++;
    }

    uintptr_t entry = loader(pcb, filename);

    ((void(*)(int, char **, char *[]))entry) (argc, pargv, penvp);
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
