#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

extern PCB* current;
extern PCB* pcbbase;
extern uint32_t proc_cur_select;
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
		case SYS_exit :
            //Log("In exit: before load");
            //naive_uload(NULL, "/bin/init");
            //proc_ctrl = 0;
            //_yield();
            _halt(a[0]);
            panic("Should not reach here!");
            break;
		case SYS_yield: _yield(); c->GPRx = 0; break;
		case SYS_brk:
            if (mm_brk(current->cur_brk + a[1])){
                c->GPRx = -1;
            } else {
                c->GPRx = current->cur_brk;
                current->cur_brk = current->cur_brk + a[1];
            }
            //Log("max_brk = %x", current->max_brk);
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
            context_uload(&pcbbase[proc_cur_select], (char *)a[1]);
            _yield();
            c->GPRx = 0;
            //args_uload(NULL, (char *)a[1], (char **)a[2], (char **)a[3]);
            //naive_uload(NULL, (char *)a[1]);
            break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
