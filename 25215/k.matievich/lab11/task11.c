#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>


extern char **environ;


int nexecvpe(char* path, char** args, char** newenv) {

    environ = newenv;

    execvp(path, args);

    return -1;
}





int main(int argc, char* argv[]) {


    if (argc == 1) {
        fprintf(stderr, "Не задана запускаемая программа\n");
        return 0;
    }

    char** args = &argv[1];


    pid_t pid = fork();
    if (pid == 0) {

        char* newenv[] = {
            "LAB=11",
            "USER=Testing",
            NULL 
        };


        if (nexecvpe(argv[1], args, newenv) == -1) {
            exit(4);
        }

    } else {

        int stat;
        wait(&stat);
        printf("\nКод завершения процесса: %d\n", WEXITSTATUS(stat));
    }

    return 0;

}