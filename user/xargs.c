// xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run(char *program, char **args) {
    // program: string of executable program
    // args: command
    if(fork() == 0) { // child exec
        exec(program, args);
        exit(0);
    }
    return;
}

int main(int argc,char * argv[]) {

    if (argc < 2){
        printf("usage: xargs  <command>  [command args]...\n");
        exit(1);
    }

    char buf[2048]; // store all char of the command
    char *argsbuf[128]; // each cell store an invidual argument. I.e: ls -l -> argsbuf[0] -> 'ls', argsbuf[1] -> '-l'
    char **args = argsbuf;

    for (int i = 1; i < argc; i++) {
        *args = argv[i];
        args++;
    }

    char *p = buf, *pbgin = buf;
    char **pargs = args;
    while (read(0, p, 1) != 0) { // read character one by one
        if (*p == ' ' || *p == '\n') { // end of an argument
            *p = '\0';
            *(pargs++) = pbgin; // point to the next available position in the argsbuf array.
            pbgin = p+1;

            if (*p == '\n') { // run the specified command
                *pargs = 0; // pointing to begining of argsbuf array
                run(argv[1],argsbuf);
                pargs = args;
            }
        }
        p++;
    }

    if (pargs != args){ // remaining argument in the argsbuf array
        *p = '\0';
        *(pargs++) = pbgin;
        *pargs = 0;
        run(argv[1],argsbuf);
    }

    while (wait(0) != -1);
    exit(0);
}
