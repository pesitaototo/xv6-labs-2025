#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
find(char *path, char *search) {
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
    case T_DEVICE:
    case T_FILE:
      // use recursion here
      printf("this is a file\n");
      break;
    case T_DIR:
      // keep traversing
      // chdir(path);

      strcpy(buf, path);
      p = buf+strlen(buf); // p keeps track of where in the path we are
      *p++ = '/';


      // what does read do again?
      while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
          continue;
        // what does memmove do again?
        // ans. adds de.name from where p points to
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) {
          printf("find: cannot stat %s\n", buf);
          continue;
        }

        if (st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
          find(buf, search);
        }

        if (st.type == T_FILE && strcmp(de.name, search) == 0) {
          printf("%s\n", buf);
        }

      }
      break;
  }

}

/*
test

echo > b
mkdir a
echo > a/b
mkdir a/aa
echo > a/aa/b
find . b

*/

int
main(int argc, char *argv[]) {
  if (argc < 3 || argc > 3) {
    fprintf(2, "Usage: find <dir> <filename>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);

}
