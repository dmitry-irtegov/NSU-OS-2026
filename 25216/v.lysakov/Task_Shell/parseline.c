#include "shell.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

enum Token {
    T_END,
    T_PIPE,
    T_SEMI,
    T_AMP,
    T_IN,
    T_OUT,
    T_APPEND,
    T_WORD,
    T_BAD,
    T_AND_IF,
    T_OR_IF
};

static char *storage;
enum chain_operator chain_after[MAXCMDS];

static enum Token next_token(const char **input, char **output, char **word) {
    const char *s = *input;

    while (isspace((unsigned char)*s))
        ++s;

    switch (*s) {
    case '\0':
        *input = s;
        return T_END;

    case '|':
        if (s[1] == '|') {
            *input = s + 2;
            return T_OR_IF;
        }

        *input = s + 1;
        return T_PIPE;

    case '&':
        if (s[1] == '&') {
            *input = s + 2;
            return T_AND_IF;
        }

        *input = s + 1;
        return T_AMP;

    case ';':
        *input = s + 1;
        return T_SEMI;

    case '<':
        *input = s + 1;
        return T_IN;

    case '>':
        *input = s + (s[1] == '>' ? 2 : 1);
        return s[1] == '>' ? T_APPEND : T_OUT;
    }

    char *start = *output;
    char *dst = start;
    int quote = 0;

    while (*s) {
        char c = *s;

        if (!quote && (isspace((unsigned char)c) || strchr("|&<>;", c)))
            break;

        if (c == '\'' && quote != '"') {
            quote = quote == '\'' ? 0 : '\'';
            ++s;
            continue;
        }

        if (c == '"' && quote != '\'') {
            quote = quote == '"' ? 0 : '"';
            ++s;
            continue;
        }

        if (c == '\\' && quote != '\'') {
            ++s;

            if (!*s || *s == '\n') {
                fprintf(stderr, "syntax error: incomplete escape\n");
                return T_BAD;
            }

            if (quote == '"' && *s != '"' && *s != '\\' && *s != '$' &&
                *s != '`')
                *dst++ = '\\';

            *dst++ = *s++;
            continue;
        }

        *dst++ = *s++;
    }

    if (quote) {
        fprintf(stderr, "syntax error: unclosed quote\n");
        return T_BAD;
    }

    *dst++ = '\0';

    *word = start;
    *output = dst;
    *input = s;

    return T_WORD;
}

int parseline(char *line) {
    free(storage);
    storage = malloc(strlen(line) + 1);
    if (!storage) {
        perror("malloc");
        return -1;
    }

    const char *input = line;
    char *output = storage;
    int current = 0, nargs = 0;
    enum Token pending = T_END;
    enum Token last = T_END;
    int background_seen = 0;
    int has_conditions = 0;
    char *word = NULL;

    bkgrnd = 0;
    infile = outfile = appfile = NULL;
    for (int i = 0; i < MAXCMDS; ++i) {
        cmds[i].cmdflag = 0;
        cmds[i].cmdargs[0] = NULL;
        chain_after[i] = CHAIN_NONE;
    }

    for (;;) {
        enum Token t = next_token(&input, &output, &word);
        if (t == T_BAD)
            return -1;
        if (background_seen && t != T_END)
            goto syntax_error;
        if (t == T_END) {
            if (pending != T_END)
                goto syntax_error;
            if (nargs == 0) {
                if (last == T_END)
                    return 0;
                if (last == T_SEMI)
                    return current;
                goto syntax_error;
            }
            return current + 1;
        }

        if (pending != T_END) {
            if (t != T_WORD)
                goto syntax_error;
            if (pending == T_IN)
                infile = word;
            else if (pending == T_OUT)
                outfile = word;
            else
                appfile = word;
            pending = T_END;
            last = t;
            continue;
        }

        switch (t) {
        case T_WORD:
            if (nargs >= MAXARGS - 1) {
                fprintf(stderr, "too many arguments\n");
                return -1;
            }
            cmds[current].cmdargs[nargs++] = word;
            cmds[current].cmdargs[nargs] = NULL;
            break;
        case T_PIPE:
            if (nargs == 0 || current + 1 >= MAXCMDS)
                goto syntax_error;

            cmds[current].cmdflag |= OUTPIP;

            ++current;

            cmds[current].cmdflag |= INPIP;
            nargs = 0;
            break;

        case T_SEMI:
        case T_AND_IF:
        case T_OR_IF:
            if (nargs == 0 || current + 1 >= MAXCMDS)
                goto syntax_error;

            if (t == T_SEMI) {
                chain_after[current] = CHAIN_SEQ;
            } else if (t == T_AND_IF) {
                chain_after[current] = CHAIN_AND;
                has_conditions = 1;
            } else {
                chain_after[current] = CHAIN_OR;
                has_conditions = 1;
            }

            ++current;
            nargs = 0;
            break;
        case T_AMP:
            if (nargs == 0 || has_conditions)
                goto syntax_error;

            bkgrnd = 1;
            background_seen = 1;
            break;
        case T_IN:
            if (infile)
                goto syntax_error;
            pending = t;
            break;
        case T_OUT:
        case T_APPEND:
            if (outfile || appfile)
                goto syntax_error;
            pending = t;
            break;
        default:
            goto syntax_error;
        }
        last = t;
    }

syntax_error:
    fprintf(stderr, "syntax error\n");
    return -1;
}
