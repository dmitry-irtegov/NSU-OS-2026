#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **saved_environ = environ;
    environ = (char **)envp;

    execvp(file, argv); 

    environ = saved_environ;
    
    return -1;
}

int main() {
    char *new_environ[] = {
        "MY_CUSTOM_VAR=HELLO_WORLD!",
        "PATH=/usr/bin:/bin",
        NULL
    };

    char *bad_argv[] = {"non_existent_program", NULL};

    if (execvpe(bad_argv[0], bad_argv, new_environ) == -1) {
        perror("execvpe failed");
    } 

    char *good_argv[] = {"env", NULL};

    execvpe(good_argv[0], good_argv, new_environ);

    perror("execvpe failed");

    exit(EXIT_FAILURE);
}
