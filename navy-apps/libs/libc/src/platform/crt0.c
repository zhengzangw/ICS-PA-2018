#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

__attribute__((section(".text.unlikely"))) void _start(int argc, char *argv[], char *envp[]) {
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
