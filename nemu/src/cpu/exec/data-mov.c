#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
	operand_write(id_dest, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  rtl_mv(&t0, &reg_l(R_ESP));
  rtl_push(&reg_l(R_EAX));
	rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
	rtl_push(&t0);
	rtl_push(&reg_l(R_EBP));
	rtl_push(&reg_l(R_ESI));
	rtl_push(&reg_l(R_EDI));

  print_asm("pusha");
}

make_EHelper(popa) {
	rtl_pop(&reg_l(R_EDI));
	rtl_pop(&reg_l(R_ESI));
	rtl_pop(&reg_l(R_EBP));
	rtl_pop(&t0);
	rtl_pop(&reg_l(R_EBX));
	rtl_pop(&reg_l(R_EDX));
	rtl_pop(&reg_l(R_ECX));
	rtl_pop(&reg_l(R_EAX));

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_sr(R_ESP, &reg_l(R_EBP), 4);
	rtl_pop(&reg_l(R_EBP));

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
		rtl_lr(&t0, R_EAX, 2);
		rtl_msb(&t0, &t0, 2);
	  if (t0 == 1) {
				rtl_li(&t1, 0xFFFF);
				rtl_sr(R_EDX, &t1, 2);
		} else {
				rtl_li(&t1, 0);
				rtl_sr(R_EDX, &t1, 2);
		}
  }
  else {
		rtl_lr(&t0, R_EAX, 4);
		rtl_msb(&t0, &t0, 4);
		if (t0 == 1){
				rtl_li(&t1, 0xFFFFFFFF);
				rtl_sr(R_EDX, &t1, 4);
		} else {
				rtl_li(&t1, 0);
				rtl_sr(R_EDX, &t1, 4);
		}
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
		rtl_lr(&t0, R_EAX, 1);
		rtl_sext(&t0, &t0, 2);
		rtl_sr(R_EAX, &t0, 2);
  }
  else {
		rtl_lr(&t0, R_EAX, 2);
		rtl_sext(&t0, &t0, 4);
		rtl_sr(R_EAX, &t0, 4);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

make_EHelper(mov_CR2r){
    switch (id_dest->reg) {
        case 0: rtl_sr(id_src->reg, &cpu.CR0, 4); break;
        case 3: rtl_sr(id_src->reg, &cpu.CR3, 4); break;
        default: panic("mov from control register other than CR0 or CR3!");
    }
}

make_EHelper(mov_r2CR){
    switch (id_dest->reg) {
        case 0: rtl_lr(&cpu.CR0, id_src->reg, 4); break;
        case 3: rtl_lr(&cpu.CR3, id_src->reg, 4); break;
        default: panic("mov to control register other than CR0 or CR3!");
    }
    Log("CR0 = %x", cpu.CR0);
}
