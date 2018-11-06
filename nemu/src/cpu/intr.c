#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
 rtlreg_t addr = cpu.idtr.addr + NO*8;
 t2 = vaddr_read(addr, 4);
 Log("%x", t2);
		
 TODO();
}

void dev_raise_intr() {
}
