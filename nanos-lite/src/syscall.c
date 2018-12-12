#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"
extern PCB* pcbbase;

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
		case SYS_exit :
            _halt(a[1]);
            //Log("In exit: before load");
            //naive_uload(NULL, "/bin/init");
            panic("Should not reach here!");
            break;
		case SYS_yield: _yield(); c->GPRx = 0; break;
		case SYS_brk:
            Log("brk = %x", a[1]);
			c->GPRx = mm_brk(a[1]);
			break;
		case SYS_open:
		    c->GPRx = fs_open((char *)a[1], (int)a[2], (int)a[3]);
			break;
		case SYS_close:
			c->GPRx = fs_close((int)a[1]);
			break;
		case SYS_lseek:
			c->GPRx = fs_lseek((int)a[1], (off_t)a[2], (int)a[3]);
			break;
		case SYS_read:
			c->GPRx = fs_read((int)a[1], (void *)a[2], (size_t)a[3]);
			break;
		case SYS_write:
			c->GPRx = fs_write((int)a[1], (void *)a[2], (size_t)a[3]);
			break;
        case SYS_execve:
            assert(0);
            context_uload(&pcbbase[1], (char *)a[1]);
            //args_uload(NULL, (char *)a[1], (char **)a[2], (char **)a[3]);
            //naive_uload(NULL, (char *)a[1]);
            c->GPRx = 0;
            break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
