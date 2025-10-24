#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (8*4096)

int
main(int argc, char *argv[])
{
  // Your code here.
  sbrk(16);
  char *att = sbrk(DATASIZE);

  int help = strcmp("This may help.", att);
  if (help == 0) {
    // printf("HELP!\n");
    printf("%s\n", (att+16));
    
  }

  // printf("%s\n", (att));

  exit(1);
}
