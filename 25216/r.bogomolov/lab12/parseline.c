#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "shell.h"

enum token {
    END, WORD, INPUT, OUTPUT, APPEND, SEMICOLON, BACKGROUND, PIPE, ERROR
};

static int put_char(char **buffer, char *end, char ch) {
    if (*buffer == end) {
        return -1;
    }
    *(*buffer)++ = ch;
    return 0;
}

static enum token nextword(char **line, char **buffer, char *end, char **word, const char **error) {
    char *p = *line;

    while (isspace((unsigned char)*p)) {
        p++;
    }
    *line = p;
    if (*p == '\0') {
        return END;
    }

    if (strchr("<>;&|", *p)) {
        char op = *p++;
        if ((op == '&' || op == '|') && *p == op) {
            *error = "Операторы && и || пока не поддерживаются";
            return ERROR;
        }
        *line = p;
        switch (op) {
        case '<': return INPUT;
        case '>':
            if (*p == '>') {
                *line = p + 1;
                return APPEND;
            }
            return OUTPUT;
        case ';': return SEMICOLON;
        case '&': return BACKGROUND;
        default:  return PIPE;
        }
    }

    *word = *buffer;
    char quote = '\0';
    while (*p != '\0') {
        char ch = *p;

        if (quote == '\0') {
            if (isspace((unsigned char)ch) || strchr("<>;&|", ch)) {
                break;
            }
            if (ch == '\'' || ch == '"') {
                quote = ch;
                p++;
                continue;
            }
        } else if (ch == quote) {
            quote = '\0';
            p++;
            continue;
        }

        if (ch == '\\' && quote != '\'') {
            if (p[1] == '\0') {
                *error = "Обратная косая черта в конце строки";
                return ERROR;
            }

            if (quote == '\0' || strchr("$`\"\\", p[1])) {
                ch = *++p;
            }
        }

        if (put_char(buffer, end, ch) == -1) {
            *error = "Переполнен буфер разбора";
            return ERROR;
        }
        p++;
    }

    if (quote != '\0') {
        *error = "Незакрытая кавычка";
        return ERROR;
    }
    if (put_char(buffer, end, '\0') == -1) {
        *error = "Переполнен буфер разбора";
        return ERROR;
    }
    *line = p;
    return WORD;
}

int parseline(char **line) {
    static char buffer[2 * MAX_LINE];
    char *p = buffer;
    char *word = NULL;
    const char *error = "Ошибка синтаксиса или слишком много аргументов/команд";
    enum token type;
    int i = 0;

    memset(cmds, 0, sizeof(cmds));
    bkgrnd = 0;
    infile = outfile = appfile = NULL;

    while ((type = nextword(line, &p, buffer + sizeof(buffer), &word, &error)) != END) {
        if (type == ERROR) {
            goto error;
        }
        if (type == SEMICOLON || type == BACKGROUND) {
            if (cmds[i].argc == 0) {
                goto error;
            }
            bkgrnd = type == BACKGROUND;
            return i + 1;
        }

        if (type == PIPE) {
            if (cmds[i].argc == 0 || i == MAX_COMMANDS - 1) {
                goto error;
            }
            i++;
        } else if (type == INPUT || type == OUTPUT || type == APPEND) {
            char *name = NULL;
            enum token filename = nextword(line, &p, buffer + sizeof(buffer), &name, &error);
            if (filename == ERROR) {
                goto error;
            }
            if (filename != WORD) {
                error = "После перенаправления требуется имя файла";
                goto error;
            }

            if (type == INPUT) {
                if (infile != NULL) {
                    goto error;
                }
                infile = name;
            } else {
                if (outfile != NULL || appfile != NULL) {
                    goto error;
                }
                if (type == APPEND) {
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
    if (i == 0 && infile == NULL && outfile == NULL && appfile == NULL) {
        return 0;
    }

error:
    fprintf(stderr, "%s\n", error);
    return -1;
}
