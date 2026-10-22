#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

struct command cmds[MAX_COMMANDS];
int bkgrnd;
char *infile, *outfile, *appfile;

static void redirect_file(const char *name, int target, int flags) {
    int fd = open(name, flags, 0666);
    if (fd == -1) {
        perror(name);
        _exit(1);
    }
    if (dup2(fd, target) == -1) {
        perror("dup2");
        _exit(1);
    }
    if (fd != target) {
        close(fd);
    }
}

int main(void) {
    char line[MAX_LINE];

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0);

        int result = prompline(line, sizeof(line));

        if (result == 0) {
            break;
        }

        if (result == -1) {
            continue;
        }

        char *p = line;
        while (*p) {
            int ncmd = parseline(&p);
            switch (ncmd) {
            case -1:
                break;
            case 0:
                continue;
            case 1:
                break;
            default:
                fprintf(stderr, "Каналы | пока не поддерживаются\n");
                continue;
            }

            if (ncmd == -1) {
                break;
            }

            for (int i = 0; i < ncmd; i++) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    break;
                }

                if (pid == 0) {
                    if (infile != NULL) {
                        redirect_file(infile, STDIN_FILENO, O_RDONLY);
                    }
                    if (outfile != NULL) {
                        redirect_file(outfile, STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC);
                    }
                    if (appfile != NULL) {
                        redirect_file(appfile, STDOUT_FILENO, O_WRONLY | O_CREAT | O_APPEND);
                    }

                    execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);
                    perror(cmds[i].cmdargs[0]);
                    _exit(1);
                }

                if (!bkgrnd) {
                    while (waitpid(pid, NULL, 0) == -1) {
                        if (errno != EINTR) {
                            perror("waitpid");
                            break;
                        }
                    }
                }
            }
        }
    }
    return ferror(stdin) ? 1 : 0;
}