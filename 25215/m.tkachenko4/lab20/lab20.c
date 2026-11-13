
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

char *join_path(const char *dir, const char *name)
{
    size_t size = strlen(dir) + strlen(name) + 2;
    char *path = malloc(size);

    if (path == NULL)
        return NULL;

    if (strcmp(dir, ".") == 0)
        snprintf(path, size, "%s", name);
    else if (strcmp(dir, "/") == 0)
        snprintf(path, size, "/%s", name);
    else
        snprintf(path, size, "%s/%s", dir, name);

    return path;
}

int search(const char *base, const char *pattern, int *found)
{
    const char *slash = strchr(pattern, '/');
    size_t len = slash ? (size_t)(slash - pattern)
                       : strlen(pattern);

    char *part = malloc(len + 1);
    if (part == NULL) {
        perror("malloc");
        return 1;
    }

    memcpy(part, pattern, len);
    part[len] = '\0';

    DIR *dir = opendir(base);
    if (dir == NULL) {
        int error = errno;
        free(part);

        if (error == ENOTDIR || error == ENOENT)
            return 0;

        errno = error;
        perror(base);
        return 1;
    }

    int result = 0;

    if (strcmp(part, ".") == 0 ||
        strcmp(part, "..") == 0) {
        char *path = join_path(base, part);

        if (path == NULL) {
            perror("malloc");
            result = 1;
        }
        else {
            if (slash)
                result = search(path, slash + 1, found);
            else {
                puts(path);
                *found = 1;
            }
            free(path);
        }
    }
    else {
        struct dirent *entry;

        while (1) {
            errno = 0;
            entry = readdir(dir);

            if (entry == NULL) {
                if (errno != 0) {
                    perror("readdir");
                    result = 1;
                }
                break;
            }

            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
                continue;

            if (!match(part, entry->d_name))
                continue;

            char *path = join_path(base, entry->d_name);

            if (path == NULL) {
                perror("malloc");
                result = 1;
                break;
            }

            if (slash) {
                if (search(path, slash + 1, found) != 0)
                    result = 1;
            }
            else {
                puts(path);
                *found = 1;
            }

            free(path);
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        result = 1;
    }

    free(part);
    return result;
}

int main(void)
{
    char pattern[SIZE];
    int found = 0;

    printf("Enter pattern: ");
    fflush(stdout);

    if (fgets(pattern, sizeof(pattern), stdin) == NULL)
        return 1;

    size_t len = strlen(pattern);

    if (len > 0 && pattern[len - 1] == '\n') {
        pattern[--len] = '\0';
    }
    else if (len == sizeof(pattern) - 1) {
        fprintf(stderr, "Pattern is too long\n");
        return 1;
    }

    if (len == 0 || pattern[len - 1] == '/' ||
        strstr(pattern, "//") != NULL) {
        fprintf(stderr, "Invalid pattern\n");
        return 1;
    }

    const char *base = ".";
    const char *rest = pattern;

    if (pattern[0] == '/') {
        base = "/";
        rest = pattern + 1;
    }

    if (*rest == '\0')
        return 1;

    if (search(base, rest, &found) != 0)
        return 1;

    if (!found)
        puts(pattern);

    return 0;
}
