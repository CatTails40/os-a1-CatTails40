
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 512

int
run_command(char *args[], int argc)
{
  int pid, status;

  args[argc] = 0;

  pid = fork();
  if (pid < 0) {
    fprintf(2, "xargs: fork failed\n");
    return -1;
  }

  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "xargs: exec %s failed\n", args[0]);
    exit(1);
  }

  if (wait(&status) < 0) {
    fprintf(2, "xargs: wait failed\n");
    return -1;
  }

  return status;
}

int
main(int argc, char *argv[])
{
  char *args[MAXARG];
  char word[MAXLINE];
  char ch;
  int base_argc = 0;
  int word_len = 0;
  int n = 0;
  int words = 0;
  int i;
  int status;
  int failed = 0;
  int batch_mode = 0;
  int nr;

  if (argc >= 2 && strcmp(argv[1], "-n") == 0) {
    if (argc < 4 || argv[2][0] == '\0') {
      fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
      exit(1);
    }

    for (i = 0; argv[2][i] != '\0'; i++) {
      if (argv[2][i] < '0' || argv[2][i] > '9' ||
          n > 214748364 ||
          (n == 214748364 && argv[2][i] > '7')) {
        fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
        exit(1);
      }
      n = n * 10 + argv[2][i] - '0';
    }

    if (n < 1) {
      fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
      exit(1);
    }

    batch_mode = 1;
    for (i = 3; i < argc; i++)
      args[base_argc++] = argv[i];
  } else {
    for (i = 1; i < argc; i++)
      args[base_argc++] = argv[i];
  }

  if (base_argc == 0 || base_argc >= MAXARG) {
    fprintf(2, "usage: xargs [-n N] cmd [args...]\n");
    exit(1);
  }

  while (1) {
    nr = read(0, &ch, 1);

    if (nr < 0) {
      fprintf(2, "xargs: read failed\n");
      exit(1);
    }

    if (nr == 0)
      break;

    if (ch == ' ' || ch == '\t' || ch == '\n') {
      if (word_len > 0) {
        char *copy;

        word[word_len] = '\0';

        if (base_argc + words >= MAXARG - 1) {
          fprintf(2, "xargs: too many arguments\n");
          exit(1);
        }

        copy = malloc(word_len + 1);
        if (copy == 0) {
          fprintf(2, "xargs: out of memory\n");
          exit(1);
        }

        strcpy(copy, word);
        args[base_argc + words] = copy;
        words++;
        word_len = 0;
      }

      if (!batch_mode && ch == '\n' && words > 0) {
        status = run_command(args, base_argc + words);
        if (status != 0)
          failed = 1;
        words = 0;
      } else if (batch_mode && words == n) {
        status = run_command(args, base_argc + words);
        if (status != 0)
          failed = 1;
        words = 0;
      }

      continue;
    }

    if (word_len >= MAXLINE - 1) {
      fprintf(2, "xargs: input word too long\n");
      exit(1);
    }

    word[word_len++] = ch;
  }

  // Handle a final word if input did not end in whitespace.
  if (word_len > 0) {
    char *copy;

    word[word_len] = '\0';

    if (base_argc + words >= MAXARG - 1) {
      fprintf(2, "xargs: too many arguments\n");
      exit(1);
    }

    copy = malloc(word_len + 1);
    if (copy == 0) {
      fprintf(2, "xargs: out of memory\n");
      exit(1);
    }

    strcpy(copy, word);
    args[base_argc + words] = copy;
    words++;
  }

  if (words > 0) {
    status = run_command(args, base_argc + words);
    if (status != 0)
      failed = 1;
  }

  exit(failed ? 1 : 0);
}

