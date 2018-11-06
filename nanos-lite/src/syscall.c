#include "common.h"
#include "syscall.h"
extern char end;

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_exit : _halt(c->GPR2); break;
		case SYS_yield: _yield(); c->eax = 0; break;
		case SYS_write: 
				Log("write: %s", (char *)c->GPR3);
				Log("%p", &end);
				if (c->GPR2==1||c->GPR2==2){
						for (size_t i=0;i<c->GPR4;++i){
								_putc(*(char *)(c->GPR3+i));
						}
				} else {
						assert(0);
				}
				c->eax = c->GPR4;
				break;
		case SYS_brk:
				c->eax = 0;
				break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
