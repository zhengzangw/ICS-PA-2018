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
  Log("offset = %d, len = %d", offset, len);
  strncpy(buf, dispinfo+offset, len);
	printf(buf);
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  Log("offset = %u", offset);
  int x = offset/32 / screen_width();
	Log("x = %u", x);
	int y = offset/32 - x;
	Log("y = %u", y);
	draw_rect((uint32_t*)buf, x, y, 1, len);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	int width = screen_width();
	int height = screen_height();
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", width, height);
	printf(dispinfo);
}
