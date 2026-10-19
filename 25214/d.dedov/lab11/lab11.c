#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern char** environ;

int execvpe(const char* file, char** args, char** env) {
    char** curr_environ = environ;
    environ = env;
    execvp(file, args);
    // if exec failed
    environ = curr_environ;
    return -1;
}

int main() {
    // char* myenv[] = {NULL};
    char* myenv[] = {"TZ=Europe/Moscow", NULL};
    char* args[] = {"./time", NULL};
    if (execvpe("./time", args, myenv) == -1) {
        perror("execvpe");
        exit(EXIT_FAILURE);
    }
    return 0;
}
