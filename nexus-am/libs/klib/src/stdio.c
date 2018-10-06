#include "klib.h"
#include <stdarg.h>

#ifndef __ISA_NATIVE__

int printf(const char *fmt, ...) {
  TODO();
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  TODO();
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  TODO();
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  TODO();
  return 0;
}

#endif
