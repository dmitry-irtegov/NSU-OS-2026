#ifndef SHELL_H
#define SHELL_H

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 16

struct command {
    char *cmdargs[MAX_ARGS];
    int argc;
};

extern struct command cmds[MAX_COMMANDS];
extern int bkgrnd;
extern char *infile, *outfile, *appfile;

int prompline(char *line, int size);

int parseline(char **line);

#endif
