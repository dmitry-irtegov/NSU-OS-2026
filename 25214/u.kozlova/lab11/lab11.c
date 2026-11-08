#include <stdio.h>
#include <unistd.h>
#include <errno.h>

extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[])
{
    char **old;

    if (file == NULL || argv == NULL || envp == NULL) {
        errno = EINVAL;
        return -1;
    }

    old = environ;
    environ = (char **)envp;
    execvp(file, argv);
    environ = old;
    return -1;
}

int main(void)
{
    char *argv[] = {"cat", "lab11.txt", NULL};
    char *envp[] = {"PATH=/usr/bin:/bin", NULL};

    if (execvpe("cat", argv, envp) == -1) {
        perror("execvpe");
        return 1;
    }
    return 0;
}
