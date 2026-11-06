#ifndef SHELL_H
#define SHELL_H

#include <signal.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define MAX_COMMANDS 16
#define MAX_JOBS 64

struct command {
    /* Массив аргументов завершается NULL и передаётся непосредственно в execvp. */
    char *cmdargs[MAX_ARGS];
    int argc;
};

extern struct command cmds[MAX_COMMANDS];
/* Результат разбора одного конвейера; перенаправления относятся к его краям. */
extern int bkgrnd;
extern char *infile, *outfile, *appfile;

extern int shell_interactive;
extern volatile sig_atomic_t input_interrupted;
int collect_jobs(void);

/* 1 — строка, 0 — EOF, -1 — переполнение, -2 — ошибка ввода, -3 — Ctrl+C. */
int prompline(char *line, int size);

/* Разбирает один конвейер и сдвигает *line; возвращает число команд или -1. */
int parseline(char **line);

#endif
