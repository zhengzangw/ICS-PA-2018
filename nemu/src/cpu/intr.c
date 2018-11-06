#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
 Log("%u", NO);
 rtlreg_t addr = cpu.idtr.addr;
 rtlreg_t tt = addr + NO*8;
 rtl_lm(&t3, &tt, 4);
 Log("addr = %u", tt);
 Log("%x", t3);

		
 TODO();
}

void dev_raise_intr() {
}
