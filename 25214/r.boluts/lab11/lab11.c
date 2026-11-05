
#include <unistd.h>
#include <stdio.h>

extern char **environ;

int execvpe(const char *file, char *const *argv, char *const *cnup) {
    char **environ_reserve_copy = environ;
    environ = (char **)cnup;
    int result = execvp(file, argv);
    environ = environ_reserve_copy;
    return result;
}

int main(void) {
    const char *file = "./example";
    char *argv[] = {"This", "is", "example", NULL};
    char *cnup[] = {"PATH=/usr/bin:/bin", "FROM=lab11", NULL};
    if (execvpe(file, argv, cnup) == -1) {
        perror("execvpe");
    }
    return 0;
}
