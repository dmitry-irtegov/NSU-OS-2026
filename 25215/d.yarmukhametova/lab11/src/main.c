#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **old_environ = environ;
    environ = (char **)envp;

    execvp(file, argv);
    int saved_errno = errno;
    environ = old_environ;
    errno = saved_errno;
    return -1;
}

int main() {
    char *argv[] = {"cat", "file", NULL};
    char *envp[] = {"PATH=/usr/bin:/bin", NULL};

    execvpe(argv[0], argv, envp);
    perror("execvpe");
    return 1;
}