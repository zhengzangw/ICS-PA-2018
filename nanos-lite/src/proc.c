#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;
PCB *pcbbase = pcb;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

bool proc_hang[MAX_NR_PROC];
uint32_t proc_cur,proc_cur_select;
bool proc_select;
void init_proc() {
    //context_kload(&pcb[1], (void *)hello_fun);
    context_uload(&pcb[0], "/bin/hello");  proc_hang[0] = true;
    context_uload(&pcb[1], "/bin/init"); proc_hang[1] = false;
    context_uload(&pcb[2], "/bin/events"); proc_hang[2] = true;
    context_uload(&pcb[3], "/bin/init"); proc_hang[3] = true;
    proc_cur = 1;
    proc_cur_select = 1;
    proc_select = true;
    switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  current->cp = prev;
  if (proc_select){
    current = &pcb[proc_cur_select];
    proc_select = false;
  } else {
    for (int i=1;i<=MAX_NR_PROC;++i){
      uint32_t proc_switch = (proc_cur+i)%MAX_NR_PROC;
      if (!proc_hang[proc_switch]){
        current = &pcb[proc_switch];
        proc_cur = proc_switch;
        break;
      }
    }
  }
  return current->cp;
}

void proc_change(uint32_t p){
  proc_hang[proc_cur_select] = true;
  proc_hang[p] = false;
  proc_cur_select = p;
  proc_select = true;
}
