#include <stdarg.h>
#include <stdio.h>
#include <sys/random.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>

void print(int lvl, const char *format, ...) {
  for (int i = 0; i < lvl; i++) {
    printf("  ");
  }
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  fflush(stdout);
}

int handle_dir(int lvl, DIR *d) {
  DIR *new;
  int ret = 0;
  for (;;) {
    struct dirent *ent = readdir(d);
    if (ent == NULL) {
      return 0;
    }

    if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0) {
      continue;
    }

    switch (ent->d_type) {
    case DT_DIR:
      print(lvl, "%s:\n", ent->d_name);
      new = opendir(ent->d_name);
      ret = handle_dir(lvl + 1, new);
      if (ret < 0) {
        return ret;
      }
      continue;
    default:
      print(lvl, "%s\n", ent->d_name);
    }
  }

  return ret;
}

int main(int argc, char *argv[]) {
  if (argc < 1) {
    fprintf(stderr, "not enough args!");
  }
  for (int i = 1; i < argc; i++) {
    print(0, "%s\n", argv[i]);

    DIR *d = opendir(argv[i]);
    int ret = handle_dir(1, d);
    if (ret < 0) {
      return ret;
    }
  }
  return 0;
}
