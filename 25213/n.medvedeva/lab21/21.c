#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t beep_counter = 0;

void work_sigint(int sig) {
    if (sig != SIGINT) {
        return;
    }

    beep_counter++;
    write(STDOUT_FILENO, "\a", 1);
    write(STDOUT_FILENO, "\n[SIGINT]Continuing...\n", 25);

    signal(SIGINT, work_sigint);
}

void work_sigquit(int sig) {
    if (sig != SIGQUIT) {
        return;
    }

    write(STDOUT_FILENO, "\n\n[SIGQUIT received]\n", 21);
    write(STDOUT_FILENO, "Total beeps: ", 13);

    char buf[16];
    int n = beep_counter;
    int i = 0;
    if (n == 0) {
        buf[i++] = '0';
    } else {
        char tmp[16];
        int j = 0;
        while (n > 0) {
            tmp[j++] = '0' + (n % 10);
            n /= 10;
        }
        while (j > 0) {
            buf[i++] = tmp[--j];
        }
    }
    buf[i++] = '\n';
    write(STDOUT_FILENO, buf, i);
    
    _exit(0);
}

int main() {
    if (signal(SIGINT, work_sigint) == SIG_ERR) {
        perror("signal SIGINT");
        return 1;
    }

    if (signal(SIGQUIT, work_sigquit) == SIG_ERR) {
        perror("signal SIGQUIT");
        return 1;
    }

    printf("Press Ctrl+C (SIGINT) to make a beep sound\n");
    printf("Press Ctrl+\\ (SIGQUIT) to exit\n");
    printf("====================================\n\n");
    
    if (fflush(stdout) == EOF) {
        perror("fflush");
        return 1;
    }

    while (1) {
        pause();
    }

}

