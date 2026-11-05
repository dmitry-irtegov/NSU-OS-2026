#include <stdio.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char *file, char *const *argv, char *const *envp){
    char **saved_environ = environ;

    environ = (char **)envp;
    execvp(file, argv);

    environ = saved_environ;
    return -1;
}

int main(void){
    char *argv[] = { "env", NULL };
    char * const envp[] = { "abc=abd", "abd=abc", NULL };

    execvpe("env", argv, envp);
    perror("execvpe");
    return 1;
}
