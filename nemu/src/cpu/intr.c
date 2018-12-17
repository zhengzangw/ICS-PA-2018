#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
// Push eflags, cs, ret_addr
 rtl_push(&cpu.eflags);
 cpu.IF = 0;
 rtl_push(&cpu.cs);
 rtl_push(&ret_addr);
//Get idt address
 rtlreg_t addr = cpu.idtr.addr + NO*8;
 assert(addr < cpu.idtr.addr+cpu.idtr.size);
//Get Gate descriptor
 uint32_t gatedesc_lo = vaddr_read(addr, 4);
 uint32_t gatedesc_hi = vaddr_read(addr + 4, 4);
//Get offset
 assert(gatedesc_hi & 8000);
 uint32_t offset = (gatedesc_lo & 0xffff) + (gatedesc_hi & 0xffff0000);
//Jump
 rtl_j(offset);
}

void dev_raise_intr() {
  cpu.INTR = true;
}
