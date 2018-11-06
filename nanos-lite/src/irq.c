#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  assert(0);
  Log("event = %#x", e.event);
  switch (e.event) {
		case _EVENT_SYSCALL: printf("SYSCALL event\n"); break;
		case _EVENT_YIELD: printf("YIELD event\n"); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
