#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int my_execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **old_environ = environ;

    environ = (char **)envp;
    execvp(file, argv);

    environ = old_environ;
    return -1;
}

int main(void) {
    char *custom_env[] = {
        "MY_SECRET_VAR=Solaris",
        "USER=GhostofIrtegov",
        "PATH=/usr/bin:/bin",
        NULL
    };

    char *args[] = { "env", NULL };

    printf("Тестируем my_execvpe() запуском команды 'env' с кастомным окружением:\n");
    printf("--------------------------------------------------------------------\n");
    fflush(stdout);

    if (my_execvpe("env", args, custom_env) == -1) {
        perror("my_execvpe не удался");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}