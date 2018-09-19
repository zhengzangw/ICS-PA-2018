#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define choose(x) (rand()%x)
// this should be enough
static char buf[65536];
static inline void gen_rand_op(){
   switch (choose(4)){
	    case 0: strcat(buf,"+"); break;
		case 1: strcat(buf,"-"); break;
		case 2: strcat(buf,"*"); break;
		default: strcat(buf,"/");
   }
}

static inline void gen_rand_num(){
	char tmp[32];
   	sprintf(tmp, "%u", rand());
	switch(choose(2)){
		case 0:
		   	strcat(buf, tmp);
			break;
		default:   	
			strcat(buf, "(-");
			strcat(buf, tmp);
			strcat(buf,")");
	}
}

static inline void gen_rand_expr() {
	switch (choose(3)){
		case 0: gen_rand_num(); break;
		case 1: strcat(buf,"("); gen_rand_num(); strcat(buf,")"); break;
		default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result =(unsigned) %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0] = '\0';
	gen_rand_expr();
	
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
