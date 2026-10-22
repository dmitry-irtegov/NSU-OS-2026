#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include "shell.h"
char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char *argv[]) {
    register int i;
    char line[1024];      /*  allow large command lines  */
    int ncmds;
    char prompt[50];      /* shell prompt */
    /* PLACE SIGNAL CODE HERE */
    sprintf(prompt,"[%s] ", argv[0]);
    while (promptline(prompt, line, sizeof(line)) > 0) {    /* until eof  */
        if ((ncmds = parseline(line)) <= 0)
            continue;   /* read next line */
        
        #ifdef DEBUG 
        {
            int i, j;
                for (i = 0; i < ncmds; i++) {
                for (j = 0; cmds[i].cmdargs[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmd[%d].cmdargs[%d] = %s\n",
                    i, j, cmds[i].cmdargs[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i, cmds[i].cmdflag);
            }
        }
        #endif

        for (i = 0; i < ncmds; i++) {
            int pid = fork();
            if (pid < 0) {
                perror("fork");
                continue;
            }

            /* Child process */
            if (pid == 0) {
                    /* Input redirection */
                if (i == 0 && infile != NULL) {
                    int fd = open(infile, O_RDONLY);

                    if (fd < 0) {
                        perror("open infile");
                        _exit(1);
                    }

                    if (dup2(fd, STDIN_FILENO) < 0) {
                        perror("dup2");
                        _exit(1);
                    }

                    close(fd);
                }

                /* Output redirection */
                if (i == ncmds - 1 && outfile != NULL) {
                    int fd = open(outfile,
                                O_WRONLY | O_CREAT | O_TRUNC,
                                0666);

                    if (fd < 0) {
                        perror("open outfile");
                        _exit(1);
                    }

                    if (dup2(fd, STDOUT_FILENO) < 0) {
                        perror("dup2");
                        _exit(1);
                    }

                    close(fd);
                }

                /* Append redirection */
                if (i == ncmds - 1 && appfile != NULL) {
                    int fd = open(appfile,
                                O_WRONLY | O_CREAT | O_APPEND,
                                0666);

                    if (fd < 0) {
                        perror("open appfile");
                        _exit(1);
                    }

                    if (dup2(fd, STDOUT_FILENO) < 0) {
                        perror("dup2");
                        _exit(1);
                    }

                    close(fd);
                }

                /* Execute command */
                execvp(cmds[i].cmdargs[0], cmds[i].cmdargs);

                perror("execvp");
                _exit(127);
            }

            /* Parent process */
            if (bkgrnd) {
                printf("%ld\n", (long)pid);
                fflush(stdout);
            } else {
                waitpid(pid, NULL, 0);
            }
        }
    }  /* close while */
}
/* PLACE SIGNAL CODE HERE */