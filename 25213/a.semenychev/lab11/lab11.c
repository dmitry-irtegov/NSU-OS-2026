#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

extern char **environ;


int execvpe(const char *file, char *const argv[], char *const envp[]) {
    static char *empty_env[] = {NULL};
    char **old_environ;

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

    environ = old_environ;
    return -1;
}


int main(int argc, char *argv[]) {
    char *new_envp[2];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s program [args...]\n", argv[0]);
        return 1;
    }

    new_envp[0] = "PATH=/bin:/usr/bin";
    new_envp[1] = NULL;

    execvpe(argv[1], &argv[1], new_envp);

    perror(argv[1]);
    return 1;
}

