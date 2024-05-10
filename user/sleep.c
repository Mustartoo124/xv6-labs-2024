// sleep.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // argc: the number of command-line arguments passed to the program
    // *argv[]: array containing the actual command line passed to the program

    if (argc != 2) {  // argc will be 2 if users provides one argument(number of ticks)
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    int ticks = atoi(argv[1]); // the number of ticks
    if (ticks <= 0) {
        fprintf(2, "Invalid argument: Please provide a positive number of ticks\n");
        exit(1);
    }
    sleep(ticks);
    exit(0);
}