#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;
static int execvpe(const char *file, char *const argv[], char *envp[])
{
    char **old_environ = environ;

    environ = envp;
    execvp(file, argv);

    environ = old_environ;
    return -1;
}

int main(void)
{
    char *args[] = {"env", NULL};
    char *envp[] = {"PATH=/usr/bin:/bin", "LAB11=demo", NULL};

    execvpe("env", args, envp);
    perror("execvpe");
    return EXIT_FAILURE;
}
