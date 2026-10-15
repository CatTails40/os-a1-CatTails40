#include "kernel/types.h"
#include "kernel/uproc.h"
#include "user/user.h"

int
main(void)
{
  struct uproc p[2];
  int n;

  n = getprocs(p, 2);
  printf("getprocs(p, 2) = %d\n", n);

  n = getprocs(p, 0);
  printf("getprocs(p, 0) = %d\n", n);

  n = getprocs(p, -1);
  printf("getprocs(p, -1) = %d\n", n);

  exit(0);
}
