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
  if (current->max_brk < new_brk){
    uint32_t szneed = new_brk - current->max_brk;
    uint32_t pgnum = szneed / PGSIZE;
    for (int i=0; i < pgnum; ++i){
        //void *pa = new_page(1);
        panic("new_brk = %x", new_brk);
        //_map(current->as, va, pa, 1);
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
