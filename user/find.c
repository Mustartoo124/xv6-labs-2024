#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Extract the base name of a file or directory from its path
char* 
fmtname(char *path)
{
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;

  return p;
}

void
find(char *path, char *targetname)
{
    // path: A pointer to a string representing the directory path from which the search will start.
    // targetname: A pointer to a string representing the name of the file to search for.
  char buf[512], *p;
  int fd;
  struct dirent de; // inode entry struct
  struct stat st;

  // target file has been found
  if(!strcmp(fmtname(path), targetname)){
    printf("%s\n", path);
  }

  // ensuring the fd is valid
  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open [%s], fd=%d\n", path, fd);
    return;
  }

  // retrieve information and check for any errors
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // check if the file type is not directory
  if(st.type != T_DIR){
    close(fd);
    return;
  }

  // path too long(over size of buffer) -> error message
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("find: path too long\n");
    close(fd);
    return;
  }

  // read the directory entries one by one and check if the entry is
  // directory itself -> recursion
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  while(read(fd, &de, sizeof(de)) == sizeof(de)){ // reading directories entry
    if(de.inum == 0) // unused entry -> pass
      continue;
    memmove(p, de.name, DIRSIZ); // copy the name of this directory into buf
    p[DIRSIZ] = 0; // set the character after the copied name to 0 -> ensures null-terminated case

    if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) // parent directory
      continue;

    find(buf, targetname); // recur if parent
  }
  close(fd);
}

int 
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "usage: find path filename\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}

