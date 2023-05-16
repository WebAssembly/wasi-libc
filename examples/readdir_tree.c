#include <stdio.h>
#include <sys/random.h>
#include <unistd.h>

#include <dirent.h>


int print_dir() {
  return 0;
}

int main() {
  char *name = "/usr";
  printf("main: just before opening dir '%s'\n", name);
  fflush(stdout);
  DIR *d = opendir(name);
  printf("main: just after opening dir '%s'\n", name);
  fflush(stdout);
  for (;;) {
    printf("main: top of for loop\n");
    fflush(stdout);
    struct dirent *ent = readdir(d);
    if (ent == NULL) {
      break;
    }
    printf("main: dir entry name '%s'\n", ent->d_name);
    fflush(stdout);
  }
  return 0;
}
