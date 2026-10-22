#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"

static char *nextword(char **line, char **buffer) {

    while (isspace((unsigned char)**line)) {
        (*line)++;
    }

    if (**line == '\0') {
        return NULL;
    }

    char *word = *buffer;

    if (strchr("<>;&|", **line)) {

        char op = *(*line)++;
        *(*buffer)++ = op;

        if (op == '>' && **line == '>') {
            *(*buffer)++ = *(*line)++;
        }

    } else {
        while (**line && !isspace((unsigned char)**line) && !strchr("<>;&|", **line)) {
            *(*buffer)++ = *(*line)++;
        }
    }
    *(*buffer)++ = '\0';

    return word;
}

int parseline(char **line) {

    static char buffer[2 * MAX_LINE];
    char *p = buffer;
    char *word;
    int i = 0;

    memset(cmds, 0, sizeof(cmds));
    bkgrnd = 0;
    infile = outfile = appfile = NULL;

    while ((word = nextword(line, &p)) != NULL) {
        if (strcmp(word, ";") == 0 || strcmp(word, "&") == 0) {
            if (cmds[i].argc == 0) {
                goto error;
            }

            bkgrnd = strcmp(word, "&") == 0;
            return i + 1;
        }

        if (strcmp(word, "|") == 0) {
            if (cmds[i].argc == 0 || i == MAX_COMMANDS - 1) {
                goto error;
            }
            i++;

        } else if (strcmp(word, "<") == 0 || strcmp(word, ">") == 0 || strcmp(word, ">>") == 0) {

            char *name = nextword(line, &p);

            if (name == NULL || strchr("<>;&|", name[0])) {
                goto error;
            }

            if (word[0] == '<') {
                if (infile != NULL) {
                    goto error;
                }
                infile = name;
            } else {
                if (outfile != NULL || appfile != NULL) {
                    goto error;
                }
                if (word[1] == '>') {
                    appfile = name;
                } else {
                    outfile = name;
                }
            }

        } else {
            if (cmds[i].argc == MAX_ARGS - 1) {
                goto error;
            }
            cmds[i].cmdargs[cmds[i].argc++] = word;
            cmds[i].cmdargs[cmds[i].argc] = NULL;
        }
    }
    if (cmds[i].argc != 0) {
        return i + 1;
    }
    if (i == 0 && infile == NULL && outfile == NULL && appfile == NULL)
        return 0;

error:
    fprintf(stderr, "Ошибка синтаксиса или слишком много аргументов/команд\n");
    return -1;
}
