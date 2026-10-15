
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *
basename(char *path)
{
  char *p = path + strlen(path);

  while (p > path && *(p - 1) != '/')
    p--;

  return p;
}

// Match a filename against a pattern.
// '*' matches any sequence of characters, including an empty sequence.
int
match(char *name, char *pattern)
{
  if (*pattern == '\0')
    return *name == '\0';

  if (*pattern == '*') {
    pattern++;

    if (*pattern == '\0')
      return 1;

    while (*name) {
      if (match(name, pattern))
        return 1;
      name++;
    }

    return match(name, pattern);
  }

  if (*name == *pattern)
    return match(name + 1, pattern + 1);

  return 0;
}

// type_filter:
// 0 = any type
// T_FILE = regular files and devices
// T_DIR = directories
void
find(char *path, char *pattern, int type_filter)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    close(fd);
    exit(1);
  }

  // If the starting path is a file or device, match its name.
  if (st.type != T_DIR) {
    close(fd);

    if ((type_filter == 0 ||
         (type_filter == T_FILE &&
          (st.type == T_FILE || st.type == T_DEVICE))) &&
        match(basename(path), pattern)) {
      printf("%s\n", path);
    }

    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);

  if (p > buf && *(p - 1) != '/')
    *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    // Skip "." and ".." to avoid recursing into them.
    if (de.name[0] == '.' &&
        (de.name[1] == '\0' ||
         (de.name[1] == '.' && de.name[2] == '\0')))
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    if (stat(buf, &st) < 0)
      continue;

    // Print entries whose names match and whose types qualify.
    if ((type_filter == 0 ||
         (type_filter == T_FILE &&
          (st.type == T_FILE || st.type == T_DEVICE)) ||
         (type_filter == T_DIR && st.type == T_DIR)) &&
        match(de.name, pattern)) {
      printf("%s\n", buf);
    }

    // Recurse into directories regardless of whether their names match.
    if (st.type == T_DIR)
      find(buf, pattern, type_filter);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  int type_filter = 0;

  if (argc != 3 && argc != 5) {
    fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
    exit(1);
  }

  if (argc == 5) {
    if (strcmp(argv[3], "-type") != 0) {
      fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
      exit(1);
    }

    if (strcmp(argv[4], "f") == 0) {
      type_filter = T_FILE;
    } else if (strcmp(argv[4], "d") == 0) {
      type_filter = T_DIR;
    } else {
      fprintf(2, "usage: find <dir> <pattern> [-type f|d]\n");
      exit(1);
    }
  }

  find(argv[1], argv[2], type_filter);
  exit(0);
}

