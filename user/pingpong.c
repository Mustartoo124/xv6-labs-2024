#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  char recv_buf[128];

  pipe(p);

  // size of ping or pong is 4 bytes so read, write 4 bytes
  // parent send ping to child process first
  if(fork() == 0){
      // child process
    read(p[0], recv_buf, 4);
    close(p[0]);

    printf("%d: received %s\n", getpid(), recv_buf);

    write(p[1], "pong", 4);
    close(p[1]);

    exit(0);
  } else{
      // parent process
    write(p[1], "ping", 4);
    close(p[1]);

    read(p[0], recv_buf, 4);
    close(p[0]);

    printf("%d: received %s\n", getpid(), recv_buf);

    exit(0);
  }
}
