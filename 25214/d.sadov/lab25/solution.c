#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);

        FILE *in = fdopen(pipefd[0], "r");
        if (!in) {
            perror("fdopen failed");
            exit(EXIT_FAILURE);
        }

        int c;

        while ((c = fgetc(in)) != EOF) {
            if (putchar(toupper(c)) == EOF) {
                perror("putchar failed");
                fclose(in);
                _exit(EXIT_FAILURE);
            }
        }

        putchar('\n');

        fclose(in);
    } else {
        close(pipefd[0]);

        FILE *out = fdopen(pipefd[1], "w");
        if (!out) {
            perror("fdopen failed");
            exit(EXIT_FAILURE);
        }

        if (fputs(argv[1], out) == EOF) {
            perror("fputs failed");
            fclose(out);
            exit(EXIT_FAILURE);
        }

        if (fclose(out) == EOF) {
            perror("fclose failed");
            exit(EXIT_FAILURE);
        }

        if (wait(NULL) == -1) {
            perror("wait failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
