#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t beep_counter = 0;
volatile sig_atomic_t write_error = 0;
volatile sig_atomic_t saved_errno = 0;

void work_sigint(int sig) {
    (void)sig;

    if (write(STDOUT_FILENO, "\a", 1) != 1) {
        saved_errno = errno;
        write_error = 1;
        return;
    }

    beep_counter++;    
}

void work_sigquit(int sig) {
    (void)sig;
    const char message[] = "\nTotal beeps: ";

    if (write(STDOUT_FILENO, message, sizeof(message) - 1) != sizeof(message) - 1) {
        saved_errno = errno;
        write_error = 1;
        return;
    }

    char buf[sizeof(sig_atomic_t) * 8 + 1];
    int i = sizeof(buf);
    sig_atomic_t n = beep_counter;

    i--;
    buf[i] = '\n';

    while (n > 0) {
        i--;
        buf[i] = '0' + n % 10;
        n /= 10;
    }

    if (beep_counter == 0) {
        i--;
        buf[i] = '0';
    }

    if (write(STDOUT_FILENO, buf + i, sizeof(buf) - i) != sizeof(buf) - i) {
        saved_errno = errno;
        write_error = 1;
        return;
    }

    _exit(0);
}

int main(void) {

    sigset_t blocked;
    sigset_t oldmask;

    if (sigemptyset(&blocked) == -1) {
        perror("sigemptyset");
        return 1;
    }

    if (sigaddset(&blocked, SIGINT) == -1 ||
        sigaddset(&blocked, SIGQUIT) == -1) {
        perror("sigaddset");
        return 1;
    }

    if (sigprocmask(SIG_BLOCK, &blocked, &oldmask) == -1) {
        perror("sigprocmask");
        return 1;
    }

    const char instructions[] =
    "Press Ctrl+C (SIGINT) to make a beep sound\n"
    "Press Ctrl+\\ (SIGQUIT) to exit\n"
    "====================================\n\n";

    if (write(STDOUT_FILENO, instructions, sizeof(instructions) - 1) != sizeof(instructions) - 1) {
        perror("write");
        return 1;
    }

    struct sigaction sa_int = {0};
    sa_int.sa_handler = work_sigint;

    if (sigemptyset(&sa_int.sa_mask) == -1) {
        perror("sigemptyset");
        return 1;
    }

    if (sigaddset(&sa_int.sa_mask, SIGQUIT) == -1) {
        perror("sigaddset");
        return 1;
    }

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }

    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = work_sigquit;
    
    if (sigemptyset(&sa_quit.sa_mask) == -1) {
        perror("sigemptyset");
        return 1;
    }

    if (sigaddset(&sa_quit.sa_mask, SIGINT) == -1) {
        perror("sigaddset");
        return 1;
    }

    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return 1;
    }

    while (1) {
        if (write_error) {
            errno = saved_errno;
            perror("write");
            return 1;
        }

        sigsuspend(&oldmask);
    }

    return 0;
}
