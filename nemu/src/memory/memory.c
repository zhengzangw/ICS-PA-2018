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
bool is_write;
//#define CROSS_PGBOUND(addr,len) ((addr>>12)!=((addr+len-1)>>12))
#define CROSS_PGBOUND(addr,len) ((addr&0xfff)+len-1>0xfff)
#define SET_DIRTY(x) (is_write?((x)|0x40):(x))
#define SET_ACCESS(x) ((x)|0x20)


paddr_t page_translation(vaddr_t addr){
    Vaddr vaddr;
    vaddr.value = addr;
    uint32_t pdir   = (cpu.CR3 & ~0xfff) + (vaddr.dir << 2);
    uint32_t pdir_index = paddr_read(pdir, 4);

    uint32_t bk_pdir = SET_ACCESS(pdir_index);
    paddr_write(pdir, bk_pdir, 4);

    Assert(pdir_index&0x1, "CR3 = %x, addr = %x, pdir = %x, pdir_index = %x, dir = %x", cpu.CR3, addr, pdir, pdir_index, vaddr.dir);

    uint32_t pte    = (pdir_index & ~0xfff) + (vaddr.page << 2);
    uint32_t pte_index  = paddr_read(pte, 4);

    uint32_t bk_pte  = SET_DIRTY(SET_ACCESS(pte_index));
    paddr_write(pte, bk_pte, 4);

    Assert(pte_index&0x1, "CR3 = %x, addr = %x, pte = %x, pte_index = %x, dir = %x", cpu.CR3, addr, pte, pte_index, vaddr.dir);
    paddr_t paddr   = (pte_index & ~0xfff) | vaddr.offset;
    return paddr;
}


uint32_t vaddr_read(vaddr_t addr, int len) {
  //if (addr>0x8000000) Log("raddr = %x", addr);
  is_write = 0;
  if (PG) {
      if (CROSS_PGBOUND(addr, len)){
        uint32_t lo_len = 0x1000 - (addr&0xfff);
        uint32_t hi_len = len - lo_len;
        paddr_t lo_paddr = page_translation(addr);
        paddr_t hi_paddr = page_translation(addr+lo_len);
        uint32_t lo = paddr_read(lo_paddr, lo_len);
        uint32_t hi = paddr_read(hi_paddr, hi_len);
        return lo + (hi<<(lo_len*8));
      } else {
          paddr_t paddr = page_translation(addr);
          return paddr_read(paddr, len);
      }
  } else {
    //Log("Unportect");
    return paddr_read(addr, len);
  }
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  //if (addr>0x8000000) Log("waddr = %x", addr);
  is_write = 1;
  if (PG) {
      if (CROSS_PGBOUND(addr, len)){
        uint32_t lo_len = 0x1000 - (addr&0xfff);
        uint32_t hi_len = len - lo_len;
        paddr_t lo_paddr = page_translation(addr);
        paddr_t hi_paddr = page_translation(addr+lo_len);
        paddr_write(lo_paddr, data & ~((0xffffffff)<<(8*lo_len)), lo_len);
        paddr_write(hi_paddr, data >> (8*lo_len), hi_len);
      } else {
        paddr_t paddr = page_translation(addr);
        paddr_write(paddr, data, len);
      }
  } else {
    //Log("Unportect");
    paddr_write(addr, data, len);
  }
}
