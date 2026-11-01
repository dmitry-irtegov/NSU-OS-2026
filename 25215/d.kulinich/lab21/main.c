#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/* 
 * volatile prohibits to compiler optimize this variable
 * sig_atomic_t garant ops of read and write be atomic
 */
volatile sig_atomic_t beep_count = 0;
volatile sig_atomic_t quit_flag = 0;


void sigint_handler(int sig) {
    (void)sig; 
    beep_count++;
    write(STDOUT_FILENO, "\nBell\n",6);
}


void sigquit_handler(int sig) {
    (void)sig;
    quit_flag = 1;
}

int main(void) {
    struct sigaction sa_int, sa_quit;

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask); 
    sa_int.sa_flags = 0;

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("failed to set handler to SIGINT");
        return 1;
    }

    sa_quit.sa_handler = sigquit_handler;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;

    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("failed to set handler to SIGQUIT");
        return 1;
    }

    while (!quit_flag) {
        pause();
    }

    printf("\nBells count %d .\n", beep_count);

    return 0;
}