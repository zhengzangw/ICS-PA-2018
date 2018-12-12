#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
  //Log("in mm_brk");
  //Log("new_brk = %x, max_brk = %x", new_brk, current->max_brk);
  //Log("cur_brk = %x", current->cur_brk);
  if (current->max_brk < new_brk){
    uint32_t new_brk_align = (new_brk & ~0xfff) + 0x1000;
    //Log("new_brk_align = %x", new_brk_align);
    //assert(0);
    uint32_t szneed = new_brk_align - current->max_brk;
    uint32_t pgnum = szneed / PGSIZE;
    uintptr_t va = current->max_brk;
    for (int i=0; i < pgnum; ++i){
        void *pa = new_page(1);
        _map(&current->as, (void* )va, pa, 1);
        va += PGSIZE;
    }
    current->max_brk = new_brk_align;
  }
  current->cur_brk = new_brk;
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
