#ifndef SHELL_H
#define SHELL_H

#include <signal.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 16
#define MAX_JOBS 64

struct command {
    char *cmdargs[MAX_ARGS];
    int argc;
};

extern struct command cmds[MAX_COMMANDS];
extern int bkgrnd;
extern char *infile, *outfile, *appfile;

extern int shell_interactive;
extern volatile sig_atomic_t input_interrupted;
int collect_jobs(void);

int prompline(char *line, int size);

int parseline(char **line);

#endif
