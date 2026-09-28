#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t beep_counter = 0;
volatile sig_atomic_t got_sigint = 0;
volatile sig_atomic_t got_sigquit = 0;

void work_sigint(int sig) {
    (void)sig;
    beep_counter++;
    got_sigint = 1;
}

void work_sigquit(int sig) {
    (void)sig;
    got_sigquit = 1;
}

int main(void) {
    struct sigaction sa_int = {0};
    sa_int.sa_handler = work_sigint;
    sigemptyset(&sa_int.sa_mask);

    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }
    
    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = work_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    
    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("sigaction SIGQUIT");
        return 1;
    }

    printf("Press Ctrl+C (SIGINT) to make a beep sound\n");
    printf("Press Ctrl+\\ (SIGQUIT) to exit\n");
    printf("====================================\n\n");
    fflush(stdout);

    while (1) {
        pause();
         if (got_sigint) {
            got_sigint = 0;
            printf("\a\n[SIGINT] Continuing...\n");
            fflush(stdout);
        }

        if (got_sigquit) {
            printf("\n\n[SIGQUIT received]\n");
            printf("Total beeps: %d\n", (int)beep_counter);
            fflush(stdout);
            _exit(0);
        }
    }

    return 0;
}
