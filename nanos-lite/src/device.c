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
  int key = read_key();
	if (key) {
			if (key&0x8000){ 
			}
			else{
					sprintf(buf, "kd RETURN\n");
				 	sprintf(buf, "kd %s\n", keyname[key]);
			}
	} else {
			sprintf(buf, "t %u\n", uptime());
	}
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //Log("offset = %d, len = %d", offset, len);
  strncpy(buf, dispinfo+offset, len);
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
	int offset_m = offset / 4;
  int x = offset_m % screen_width();
	int y = offset_m / screen_width();
	draw_rect((uint32_t*)buf, x, y, len/4, 1);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
	Log("VGA WIDTH:%d HEIGHT:%d", screen_width(), screen_height());
}
