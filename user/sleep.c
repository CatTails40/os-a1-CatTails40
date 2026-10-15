
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int ticks = 0;
  int i;

  if (argc != 2) {
    fprintf(2, "usage: sleep <ticks>\n");
    exit(1);
  }

  for (i = 0; argv[1][i] != '\0'; i++) {
    if (argv[1][i] < '0' || argv[1][i] > '9') {
      fprintf(2, "usage: sleep <ticks>\n");
      exit(1);
    }

    if (ticks > 214748364 ||
        (ticks == 214748364 && argv[1][i] > '7')) {
      fprintf(2, "usage: sleep <ticks>\n");
      exit(1);
    }

    ticks = ticks * 10 + (argv[1][i] - '0');
  }

  pause(ticks);
  exit(0);
}

