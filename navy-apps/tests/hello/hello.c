#include <unistd.h>
#include <stdio.h>

int main() {
		printf("%d", 10);
  write(1, "Hello World!\n", 13);
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
				i++;
      printf("Hello World from Navy-apps for the %dth time!\n", 10);
      j = 0;
    }
  }
  return 0;
}
