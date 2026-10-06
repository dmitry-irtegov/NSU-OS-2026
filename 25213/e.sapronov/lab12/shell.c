#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "shell.h"

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char *argv[]) {
    register int i;
    char line[1024];
    int ncmds;
    char prompt[50];

    snprintf(prompt, 50, "[%s] ", argv[0]);

    while (promptline(prompt, line, sizeof(line)) > 0) {
        if ((ncmds = parseline(line)) <= 0)
            continue;

#ifdef DEBUG
        {
            int i, j;

            for (i = 0; i < ncmds; i++) {
                for (j = 0;
                     cmds[i].cmdargs[j] != (char *) NULL;
                     j++) {

                    fprintf(stderr,
                            "cmd[%d].cmdargs[%d] = %s\n",
                            i, j, cmds[i].cmdargs[j]);
                }

                fprintf(stderr,
                        "cmds[%d].cmdflag = %o\n",
                        i, cmds[i].cmdflag);
            }
        }
#endif

        for (i = 0; i < ncmds; i++) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("fork");
                continue;
            }

            if (pid == 0) {
                execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);

                perror(cmds[i].cmdargs[0]);
                exit(1);
            }

            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}

