#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;
volatile sig_atomic_t quit = 0;

void sigint_handler(int sig) {
    (void)sig;
    count++;
    write(STDOUT_FILENO, "\a", 1);
}

void sigquit_handler(int sig) {
    (void)sig;
    quit = 1;
}

int main(void) {
    struct sigaction sa_int = {0};
    struct sigaction sa_quit = {0};

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);

    sa_quit.sa_handler = sigquit_handler;
    sigemptyset(&sa_quit.sa_mask);

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (!quit) {
        pause();
    }

    printf("\nSignal sound: %d times\n", (int)count);

    return 0;
}