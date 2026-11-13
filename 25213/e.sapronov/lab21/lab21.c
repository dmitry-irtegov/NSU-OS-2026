#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t count;
volatile sig_atomic_t quit;

void handle_sigint(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\a", 1);
    count++;
}

void handle_sigquit(int sig)
{
    (void)sig;
    quit = 1;
}

int main(void)
{
    if (sigset(SIGINT, handle_sigint) == SIG_ERR) {
        perror("sigset");
        exit(1);
    }

    if (sigset(SIGQUIT, handle_sigquit) == SIG_ERR) {
        perror("sigset");
        exit(1);
    }

    while (!quit) {
        sleep(1);
    }

    printf("\ntotal beeps: %d\n", count);
    return 0;
}
