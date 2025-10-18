#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
find(char *path, char *search, char *argv[], int argc) {
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
    case T_DIR:
      strcpy(buf, path);
      p = buf+strlen(buf); // p keeps track of where in the path we are
      *p++ = '/';


      // read directory information
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
          find(buf, search, argv, argc);
        }

        // file was found
        if (st.type == T_FILE && strcmp(de.name, search) == 0) {
          if (argc <= 3) {
            printf("%s\n", buf);
          } else {
            // fork, wait, exec
            int pid = fork(); // fork returns 0 to child, but pid to parent
            if (pid == 0) {
              char *execArgs[28];
              
              int i=0;
              for(i=0; argv[i] && i < 28; i++) {
                execArgs[i] = argv[i];
              }
              execArgs[i] = buf;
              execArgs[i+1] = 0;
              
              // printf("running: %s %s %s\n", execArgs[0], execArgs[1], execArgs[2]);
              exec(execArgs[0], execArgs);
              exit(0);
            } else {
              pid = wait((int*)0);
            }
          }
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
  if (argc < 3 || argc > 32
    || (argc > 3 && strcmp("-exec", argv[3]) != 0)) {
    fprintf(2, "Usage: find <dir> <filename> [optional] -exec <commands ...>\n");
    exit(1);
  }

  find(argv[1], argv[2], (argv+4), argc);
  exit(0);

}
