#include <am.h>
#include <x86.h>
#include <amdev.h>

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
			uint32_t code = inl(0x60);
			if (code & 0x8000) {
					// make code
					kbd->keycode = code ^ 0x8000;
					kbd->keydown = 1;
			} else {
					// break code
					kbd->keycode = _KEY_NONE;
					kbd->keydown = 0;
			}

      kbd->keydown = 0;
      kbd->keycode = _KEY_NONE;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
