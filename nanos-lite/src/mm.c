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
extern PCB* current;
int mm_brk(uintptr_t new_brk) {
  while (current->max_brk <= new_brk){
    void *pa = new_page(1);
    _map(&current->as, (void *)current->max_brk, pa, 1);
    current->max_brk += PGSIZE;
  }
  uintptr_t tmp = current->max_brk;
  Log("max_brk = %x", tmp);
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
