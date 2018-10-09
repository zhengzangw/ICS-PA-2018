#include <am.h>
#include <x86.h>
#include <amdev.h>

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
			uint8_t tmp = inb(0x48);
			uint8_t pre = uptime->lo & 0xff;
			uint8_t high= uptime->lo >> 8;
			if (tmp<pre) {
					if (high==0xff) uptime->hi += 1;
					high = 0;
			} else {
					high += 1;
			}
			uptime->lo = high << 8 | tmp;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
}
