#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nemu.h"
#include "monitor/expr.h"

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* Test */
  Log("Begin Test");
  FILE *fp = fopen("~/ics2018/nemu/tools/input","r");
  assert(fp==NULL);
  char str[1000];
  char *ans,*arg;
  bool success = true;

  char test = fgetc(fp);
  Log("Before Enter");
  Log("%c",test);
  while (fgets(str,100,fp)!=NULL){
	Log("str = %s", str);
	Log("New Turn");
	ans = strtok(str," ");
	arg = strtok(NULL," ");
	uint32_t ret = expr(arg, &success);
	uint32_t retans;
	sscanf(ans,"%u", &retans);
	if (ret==retans){
		printf("Right\n");
	} else {
		printf("%u!= %u",ret,retans);
	}
  }
  fclose(fp);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
