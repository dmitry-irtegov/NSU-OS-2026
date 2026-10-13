#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

extern char **environ;


int execvpe(const char *file, char *const argv[], char *const envp[]) {
    static char *empty_env[] = {NULL};
    char **old_environ;
    int saved_errno;

    if (file == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (envp == NULL) {
        envp = (char *const *)empty_env;
    }

    old_environ = environ;
    environ = (char **)envp;

    execvp(file, argv);

    saved_errno = errno;
    environ = old_environ;
    errno = saved_errno;
    return -1;
}


int main(int argc, char *argv[]) {
    static char path_env[1024];
    char *new_envp[2];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s program [args...]\n", argv[0]);
        return 1;
    }

    snprintf(path_env, sizeof(path_env), "PATH=/bin:/usr/bin:.");

    new_envp[0] = path_env;
    new_envp[1] = NULL;

    execvpe(argv[1], &argv[1], new_envp);

    fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
    return 1;
}

