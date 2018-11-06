#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
// Push eflags, cs, eip
 rtl_push(&cpu.eflags);
 rtl_push(&cpu.cs);
 rtl_push(&cpu.eip);
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
 Log("%x",cpu.eip+offset);
 Log("%x",cpu.eip);
 //rtl_j(cpu.eip+offset);
}

void dev_raise_intr() {
}
