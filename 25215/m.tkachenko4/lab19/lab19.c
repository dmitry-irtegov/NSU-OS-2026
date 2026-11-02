
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define SIZE 4096

int match(const char *pattern, const char *name)
{
    const char *star = NULL;
    const char *saved = NULL;

    while (*name != '\0') {
        if (*pattern == '?' ||
            (*pattern != '\0' && *pattern == *name)) {
            pattern++;
            name++;
        }
        else if (*pattern == '*') {
            star = pattern;
            pattern++;
            saved = name;
        }
        else if (star != NULL) {
            pattern = star + 1;
            saved++;
            name = saved;
        }
        else {
            return 0;
        }
    }

    while (*pattern == '*')
        pattern++;

    return *pattern == '\0';
}

int main(void)
{
    char pattern[SIZE];
    DIR *dir;
    struct dirent *entry;
    size_t len;
    int found = 0;

    printf("Enter pattern: ");
    fflush(stdout);

    if (fgets(pattern, sizeof(pattern), stdin) == NULL) {
        fprintf(stderr, "Input error\n");
        return 1;
    }

    len = strlen(pattern);

    if (len > 0 && pattern[len - 1] == '\n') {
        pattern[len - 1] = '\0';
    }
    else if (len == sizeof(pattern) - 1) {
        fprintf(stderr, "Pattern is too long\n");
        return 1;
    }

    if (strchr(pattern, '/') != NULL) {
        fprintf(stderr, "Pattern cannot contain '/'\n");
        return 1;
    }

    dir = opendir(".");

    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while (1) {
        errno = 0;
        entry = readdir(dir);

        if (entry == NULL) {
            if (errno != 0) {
                perror("readdir");
                closedir(dir);
                return 1;
            }
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (match(pattern, entry->d_name)) {
            puts(entry->d_name);
            found = 1;
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        return 1;
    }

    if (!found)
        puts(pattern);

    return 0;
}
