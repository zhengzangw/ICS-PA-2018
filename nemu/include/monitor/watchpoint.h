#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next,*pre;
  char expression[64];
  bool enable;
  bool valid;
  uint32_t val;
} WP;

void wp_info();
void new_wp(char *);
void free_wp(uint32_t);
bool wp_check();
#endif
