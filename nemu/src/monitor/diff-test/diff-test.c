#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

extern int diff_on;

#define diff_error(x) do {\
		printf(#x " : %x!=%x(ref)\n", cpu.x, ref_r.x); \
		all_same = false;\
} while (0)

extern uint64_t g_nr_guest_instr;

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() {
    if (!diff_on) return;
    is_skip_ref = true;
}
void difftest_skip_dut() {
    if (!diff_on) return;
    is_skip_dut = true;
}

long memsize;
void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);
  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  memsize = img_size;
  ref_difftest_setregs(&cpu);
}

void difftest_attach(){
    ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
    ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), memsize);

    struct IDTR{
        uint16_t size;
        uint32_t addr;
    } idtr;
    idtr.size = cpu.idtr.size;
    idtr.addr = cpu.idtr.addr;
    ref_difftest_memcpy_from_dut(0x7c00, &idtr, sizeof(idtr));
    CPU_state tmp = cpu;
    int8_t inst[] = {0x0f,0x01,0xD1,0x00,0x7e,0x00,0x00};
    ref_difftest_memcpy_from_dut(0x7e40, inst, sizeof(inst));
    tmp.eip = 0x7e40;
    ref_difftest_setregs(&tmp);
    ref_difftest_exec(1);

    ref_difftest_setregs(&cpu);
}

uint64_t qemu_total_instr = 0;
void difftest_step(uint32_t eip) {
  if (!diff_on){
    return;
  }

  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
	//qemu_total_instr++;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
	//qemu_total_instr++;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);

  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
	bool all_same = true;

//Log("%#x\n",ref_cpu.eip);
  if (ref_r.eax!=cpu.eax) diff_error(eax);
  if (ref_r.ebx!=cpu.ebx) diff_error(ebx);
  if (ref_r.ecx!=cpu.ecx) diff_error(ecx);
  if (ref_r.edx!=cpu.edx) diff_error(edx);
  if (ref_r.esp!=cpu.esp) diff_error(esp);
  if (ref_r.ebp!=cpu.ebp) diff_error(ebp);
  if (ref_r.esi!=cpu.esi) diff_error(esi);
  if (ref_r.edi!=cpu.edi) diff_error(edi);
  if (ref_r.eip!=cpu.eip) diff_error(eip);
  if (!all_same) nemu_state = NEMU_ABORT;
  /* With detach, this part doesn't work
  if (qemu_total_instr!=g_nr_guest_instr) {
    printf("total instr different: %ld!=%ld", g_nr_guest_instr, qemu_total_instr);
    assert(0);
  }
  qemu_total_instr++;
  */
}

