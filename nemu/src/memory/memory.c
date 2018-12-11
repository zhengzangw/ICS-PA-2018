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

paddr_t page_translation(vaddr_t addr){
    uint32_t offset = addr & 0xfff;
    uint32_t laddr  = addr >> 12;
    uint32_t page   = laddr & 0x3ff;
    uint32_t dir    = laddr >> 10;
    uint32_t pdir   = (cpu.CR3 & ~0xfff) | (dir << 2);
    uint32_t pdir_index = paddr_read(pdir, 4);
    assert(pdir_index&0x1);
    uint32_t pte    = (pdir_index & ~0xfff) | (page << 2);
    uint32_t pte_index  = paddr_read(pte, 4);
    assert(pte_index&0x1);
    paddr_t paddr   = (pte_index & ~0xfff) | offset;
    return paddr;
}

#define CROSS_PGBOUND(addr,len) ((addr>>12)!=((addr+len)>>12))

uint32_t vaddr_read(vaddr_t addr, int len) {
  if (PG) {
      if (CROSS_PGBOUND(addr, len)){
        TODO();
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
        TODO();
      } else {
        paddr_t paddr = page_translation(addr);
        paddr_write(paddr, data, len);
      }
  } else {
    paddr_write(addr, data, len);
  }
}
