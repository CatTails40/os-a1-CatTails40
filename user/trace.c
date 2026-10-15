#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int mask = 0;

  if (argc < 3) {
    fprintf(2, "usage: trace mask command [args...]\n");
    exit(1);
  }

  for (char *s = argv[1]; *s; s++) {
    if (*s < '0' || *s > '9') {
      fprintf(2, "usage: trace mask command [args...]\n");
      exit(1);
    }

    mask = mask * 10 + (*s - '0');
  }

  if (trace(mask) < 0) {
    fprintf(2, "trace: failed\n");
    exit(1);
  }

  exec(argv[2], &argv[2]);

  fprintf(2, "trace: exec %s failed\n", argv[2]);
  exit(1);
}
