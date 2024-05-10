// primes.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int fpipe[2]) {
    int p;
    read(fpipe[0], &p, sizeof(int));

    if (p == 0) {
        exit(0);
    }
    printf("prime %d\n", p);

    int cpipe[2]; // new pipe to communicate with child process
    pipe(cpipe);

    if (fork() == 0) {
        close(fpipe[0]); // close read end of parent's pipe
        close(cpipe[1]); // close write end of its own pipe
        prime(cpipe); // find more prime num
    }
    else {
        close(cpipe[0]);
        int number;

        // filter non-prime numbers in child process
        while (1) {
            read(fpipe[0], &number, sizeof(int));
            if (number == 0) {
                break;
            }
            if (number % p) {
                write(cpipe[1], &number, sizeof(int));
            }
            // send non-prime to parent process
        }

        // writing the remaining number to the child pipe
        write(cpipe[1], &number, sizeof(int));
        close(cpipe[1]);
        close(fpipe[0]);

        // waiting the child process to finish
        wait(0);
        exit(0);
    }
    // fpipe is used for the parent to send prime numbers to the child.
    // cpipe is used for the child to send non-prime numbers back to the parent.
}

int main() {
    int fpipe[2];
    pipe(fpipe);

    if (fork() == 0) {
        close(fpipe[1]);
        prime(fpipe); // using prime in child process
    }
    else {
        close(fpipe[0]);
        int i;

        for (i = 2; i <= 35; i++) {
            write(fpipe[1], &i, sizeof(int));
            // sent number to child process
        }

        i = 0; // exit sign
        write(fpipe[1], &i, sizeof(int));
        close(fpipe[1]);
    }

    wait(0);
    exit(0);
}
