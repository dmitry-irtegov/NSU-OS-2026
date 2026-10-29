#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include "shell.h"

static void prompt(void) {
    if (shell_interactive) {
        fputs("shell> ", stdout);
        fflush(stdout);
    }
}

static int read_character(char *ch) {
    sigset_t blocked, previous;
    sigemptyset(&blocked);
    sigaddset(&blocked, SIGINT);
    sigaddset(&blocked, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &blocked, &previous) == -1) {
        return -1;
    }

    for (;;) {
        if (input_interrupted) {
            sigprocmask(SIG_SETMASK, &previous, NULL);
            return -2;
        }
        if (collect_jobs()) {
            prompt();
        }

        fd_set input;
        FD_ZERO(&input);
        FD_SET(STDIN_FILENO, &input);
        int ready = pselect(STDIN_FILENO + 1, &input, NULL, NULL, NULL, &previous);
        if (ready == -1 && errno == EINTR) {
            continue;
        }
        int saved_errno = errno;
        sigprocmask(SIG_SETMASK, &previous, NULL);
        if (ready == -1) {
            errno = saved_errno;
            return -1;
        }
        if (input_interrupted) {
            return -2;
        }

        ssize_t count = read(STDIN_FILENO, ch, 1);
        if (count == -1 && errno == EINTR) {
            if (sigprocmask(SIG_BLOCK, &blocked, NULL) == -1) {
                return -1;
            }
            continue;
        }
        return (int)count;
    }
}

int prompline(char *line, int size) {
    int used = 0;
    int too_long = 0;
    char ch;
    if (input_interrupted) {
        input_interrupted = 0;
        fputc('\n', stdout);
    }
    prompt();

    for (;;) {
        int result = read_character(&ch);
        if (result == -2) {
            input_interrupted = 0;
            fputc('\n', stdout);
            return -3;
        }
        if (result == -1) {
            perror("read");
            return -2;
        }
        if (result == 0 || ch == '\n') {
            line[used] = '\0';
            if (too_long) {
                fprintf(stderr, "Слишком длинная строка\n");
                return -1;
            }
            return result == 0 && used == 0 ? 0 : 1;
        }
        if (used < size - 1) {
            line[used++] = ch;
        } else {
            too_long = 1;
        }
    }
}
