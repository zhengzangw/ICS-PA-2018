#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case SYS_exit : _halt(c->GPR2); break;
		case SYS_yield: _yield(); c->eax = 0; break;
		case SYS_write: 
				if (c->GPR2==1||c->GPR2==2){
						for (size_t i=0;i<c->GPR4;++i){
								_putc(*((char *)c->GPR3+i));
						}
				} else {
						assert(0);
				}
				break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
