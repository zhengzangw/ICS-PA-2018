#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i=0; i<len; ++i){
			_putc(*(char *)(buf+i));
	}
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //Log("offset = %d, len = %d", offset, len);
  strncpy(buf, dispinfo+offset, len);
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  Log("offset = %u", offset);
	int offset_m = offset / 32;
  int x = offset_m % screen_height();
	Log("x = %u", x);
	int y = offset_m / screen_height();
	Log("y = %u", y);
	Log("len = %u", len/32);
	draw_rect((uint32_t*)buf, x, y, 1, len/32);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
	Log("WIDTH:%d HEIGHT:%d", screen_width(), screen_height());
}
