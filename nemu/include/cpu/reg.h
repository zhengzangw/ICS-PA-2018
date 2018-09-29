#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

typedef struct {
  union {
	  union {
	    uint32_t _32;
	    uint16_t _16;
	    uint8_t _8[2];
	  } gpr[8];
	
	  /* Do NOT change the order of the GPRs' definitions. */
	
	  struct {rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;};
  };

  vaddr_t eip;

  /*XXX: EFlags may be implemented in a better way*/
  union{
	  uint32_t EFlags;
	  struct {
		  int CF : 1;
	      int : 1;
		  int PF : 1;
		  int : 1;
		  int AF : 1;
		  int : 1;
		  int ZF : 1;
		  int SF : 1;
		  int TF : 1;
		  int IF : 1;
		  int DF : 1;
		  int OF : 1;
		  int IOPL : 2;
		  int NT : 1;
          int : 1;
		  int RF : 1;
		  int VM : 1;
	  };
  };
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
