#include "common.h"
#include "syscall.h"

#define MAX_NR_PROC 4
uint32_t time_slice[MAX_NR_PROC] = {10,1,0,0};
uint32_t time_count, cur_proc;
uint32_t total_proc = 2;

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
		case _EVENT_SYSCALL:
			return do_syscall(c);
			break;
		case _EVENT_YIELD:
            return schedule(c);
			break;
        case _EVENT_IRQ_TIMER:
            Log("receive _EVENT_IRQ_TIMER");
            time_count++;
            if (time_count == time_slice[cur_proc]){
              time_count = 0;
              cur_proc = (cur_proc + 1)%total_proc;
              _yield();
            }
            break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
