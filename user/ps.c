#include "kernel/types.h"
#include "kernel/uproc.h"
#include "user/user.h"

#define MAXPROCS 64

char *
state_name(int state)
{
  switch (state) {
  case 1: return "used";
  case 2: return "sleeping";
  case 3: return "runnable";
  case 4: return "running";
  case 5: return "zombie";
  default: return "unknown";
  }
}

int
main(void)
{
  struct uproc procs[MAXPROCS];
  int n;
  int i;

  n = getprocs(procs, MAXPROCS);
  if (n < 0) {
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }

  printf("PID       PPID STATE SIZE      NAME\n");

  for (i = 0; i < n; i++) {
    printf("%d\t%d\t%s\t%lu\t%s\n",
           procs[i].pid,
           procs[i].ppid,
           state_name(procs[i].state),
           procs[i].sz,
           procs[i].name);
  }

  exit(0);
}
