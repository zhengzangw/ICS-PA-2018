#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
 rtlreg_t addr = cpu.idtr.addr;
 t3 = addr + NO*8;
 rtl_lm(&t3, &t3, 4);
 Log("%u", t3);

		
 TODO();
}

void dev_raise_intr() {
}
