#include "cpu/exec.h"

/* control */
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jcc);
make_EHelper(jmp);
make_EHelper(call_rm);
make_EHelper(jmp_rm);

/* logic */
make_EHelper(test);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(setcc);
make_EHelper(not);
make_EHelper(sar);
make_EHelper(shr);
make_EHelper(shl);
make_EHelper(rcl);
make_EHelper(rcr);
make_EHelper(ror);
make_EHelper(rol);

/* arith */
make_EHelper(sub);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(cmp);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(neg);
make_EHelper(imul2);
make_EHelper(imul3);

/* data-mov */
make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(lea);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(leave);
make_EHelper(cltd);
make_EHelper(cwtl);

/* special */
make_EHelper(nemu_trap);
make_EHelper(inv);
make_EHelper(nop);

/* prefix */
make_EHelper(operand_size);

/* system */
make_EHelper(in);
make_EHelper(out);
make_EHelper(lidt);
make_EHelper(int);
