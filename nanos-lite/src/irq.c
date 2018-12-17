#include "common.h"
#include "syscall.h"

#define MAX_NR_PROC 4
uint32_t time_slice[MAX_NR_PROC] = {1,10,10,10};
extern uint32_t proc_cur;
extern bool proc_hang[MAX_NR_PROC];
uint32_t time_count;

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
		case _EVENT_SYSCALL:
			return do_syscall(c);
			break;
		case _EVENT_YIELD:
            return schedule(c);
			break;
        case _EVENT_IRQ_TIMER:
            //Log("receive _EVENT_IRQ_TIMER");
            time_count++;
            if (time_count == time_slice[proc_cur]){
              time_count = 0;
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
