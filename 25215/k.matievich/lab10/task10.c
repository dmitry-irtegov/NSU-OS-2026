#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>



int main(int argc, char* argv[]) {


    if (argc == 1) {
        fprintf(stderr, "Не задана запускаемая программа\n");
        return 0;
    }

    char** args = &argv[1];


    pid_t pid = fork();
    if (pid == 0) {

        if (execvp(argv[1], args) == -1) {
            exit(4);
        }

    } else {

        int stat;
        wait(&stat);
        printf("\nКод завершения процесса: %d\n", WEXITSTATUS(stat));
    }

    return 0;
}