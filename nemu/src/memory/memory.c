#include "nemu.h"

#include "device/mmio.h"
#include "cpu/reg.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int ind = is_mmio(addr);
	if (ind == -1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	else
		return mmio_read(addr, len, ind);
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int ind = is_mmio(addr);
	if (ind == -1)
    memcpy(guest_to_host(addr), &data, len);
	else
	{
		mmio_write(addr, len, data, ind);
		//printf("%u\n",data);
	}
}

typedef union {
    uint32_t value;
    struct {
        uint32_t offset : 12;
        uint32_t page : 10;
        uint32_t dir  : 10;
    };
} Vaddr;
paddr_t page_translation(vaddr_t addr){
    Vaddr vaddr;
    vaddr.value = addr;
    uint32_t pdir   = (cpu.CR3 & ~0xfff) + (vaddr.dir << 2);
    uint32_t pdir_index = paddr_read(pdir, 4);
    Assert(pdir_index&0x1, "CR3 = %x, addr = %x, pdir = %x, pdir_index = %x, dir = %x", cpu.CR3, addr, pdir, pdir_index, vaddr.dir);
    uint32_t pte    = (pdir_index & ~0xfff) + (vaddr.page << 2);
    uint32_t pte_index  = paddr_read(pte, 4);
    Assert(pte_index&0x1, "CR3 = %x, addr = %x, pte = %x, pte_index = %x, dir = %x", cpu.CR3, addr, pte, pte_index, vaddr.dir);
    paddr_t paddr   = (pte_index & ~0xfff) | vaddr.offset;
    return paddr;
}

#define CROSS_PGBOUND(addr,len) ((addr>>12)!=((addr+len-1)>>12))

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (PG) {
      if (CROSS_PGBOUND(addr, len)){
        uint32_t lo_len = ((addr+len-1)&~0xfff) - addr;
        uint32_t hi_len = (addr+len-1) - ((addr+len-1)&~0xfff);
        paddr_t lo_paddr = page_translation(addr);
        paddr_t hi_paddr = page_translation((addr+len-1)&~0xfff);
        uint32_t lo = paddr_read(lo_paddr, lo_len);
        uint32_t hi = paddr_read(hi_paddr, hi_len);
        return lo | (hi<<lo_len);
      } else {
          paddr_t paddr = page_translation(addr);
          return paddr_read(paddr, len);
      }
  } else {
    return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (PG) {
      if (CROSS_PGBOUND(addr, len)){
        uint32_t lo_len = ((addr+len-1)&~0xfff) - addr;
        uint32_t hi_len = (addr+len-1) - ((addr+len-1)&~0xfff);
        paddr_t lo_paddr = page_translation(addr);
        paddr_t hi_paddr = page_translation((addr+len-1)&~0xfff);
        paddr_write(lo_paddr, data & ~((-1)<<lo_len), lo_len);
        paddr_write(hi_paddr, data >> lo_len, hi_len);
      } else {
        paddr_t paddr = page_translation(addr);
        paddr_write(paddr, data, len);
      }
  } else {
    paddr_write(addr, data, len);
  }
}
