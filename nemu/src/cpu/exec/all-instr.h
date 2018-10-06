#include "cpu/exec.h"

/* control */
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jcc);
make_EHelper(jmp);

/* logic */
make_EHelper(test);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(setcc);
make_EHelper(not);

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

/* data-mov */
make_EHelper(mov);
make_EHelper(movzx);
make_EHelper(lea);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(leave);
make_EHelper(cltd);

/* special */
make_EHelper(nemu_trap);
make_EHelper(inv);
make_EHelper(nop);

/* prefix */
make_EHelper(operand_size);

