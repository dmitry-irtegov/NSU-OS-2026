#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int count;

void handle_sigint(int sig)
{
    (void)sig;
    write(STDOUT_FILENO, "\a", 1);
    count++;
}

void handle_sigquit(int sig)
{
    (void)sig;
    char buffer[64];
    int length = snprintf(buffer, sizeof(buffer), "\ntotal beeps: %d\n", count);
    write(STDOUT_FILENO, buffer, length);
    _exit(0);
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

    for (;;) {
        pause();
    }
}
