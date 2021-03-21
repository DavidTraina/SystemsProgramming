/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;


/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */
void handler(int i) {
  printf(MESSAGE, num_reads, seconds);
  exit(0);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    // Install signal handler
    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    if (sigaction(SIGPROF, &newact, NULL) == -1) {
      perror("sigaction");
      exit(1);
    }

    // Set Alarm (timer)
    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;


    if (setitimer(ITIMER_PROF, &timer, NULL) == -1) {
      perror("setitimer");
      exit(1);
    }


    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;num_reads++) {
      fseek(fp, (random() % 99) * sizeof(int) , SEEK_SET);
      int temp;
      fread(&temp, sizeof(int), 1, fp);
      printf("%d\n", temp);
    }
    return 1; // something is wrong if we ever get here!
}
