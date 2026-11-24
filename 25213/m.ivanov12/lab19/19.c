#define _FILE_OFFSET_BITS 64
#define __EXTENSIONS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>

int match(const char *pattern, const char *filename) {
    const char *star = NULL;
    const char *retry = NULL;

    while (*filename != '\0') {
        if (*pattern == *filename || *pattern == '?') {
            pattern++;
            filename++;
        } else if (*pattern == '*') {
            star = pattern++;
            retry = filename;
        } else if (star != NULL) {
            pattern = star + 1;
            filename = ++retry;
        } else {
            return 0;
        }
    }

    while (*pattern == '*') {
        pattern++;
    }

    return *pattern == '\0';
}

int main(void) {
    char *pattern = NULL;
    size_t capacity = 0;
    ssize_t length;

    fprintf(stderr, "Введите шаблон имени файла: ");
    if (fflush(stdout) == EOF) {
        perror("fflush");
        return 1;
    }

    length = getline(&pattern, &capacity, stdin);
    if (length == -1) {
        if (ferror(stdin)) {
            perror("getline");
        } else {
            fprintf(stderr, "Не введён шаблон.\n");
        }
        free(pattern);
        return 1;
    }

    if (length > 0 && pattern[length - 1] == '\n') {
        pattern[length - 1] = '\0';
    }

    if (strchr(pattern, '/') != NULL) {
        fprintf(stderr, "Ошибка: символ '/' не может встречаться в шаблоне.\n");
        free(pattern);
        return 1;
    }

    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Не удалось открыть текущий каталог");
        free(pattern);
        return 1;
    }

    int found = 0;
    struct dirent *entry;
    int read_error = 0;

    while (1) {
        errno = 0;
        entry = readdir(dir);

        if (entry == NULL) {
            if (errno != 0) {
                perror("readdir");
                read_error = 1;
            }
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (match(pattern, entry->d_name)) {
            printf("%s\n", entry->d_name);
            found = 1;
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        free(pattern);
        return 1;
    }

    if (read_error) {
        free(pattern);
        return 1;
    }

    if (!found) {
        printf("%s\n", pattern);
    }

    free(pattern);
    return 0;
}
