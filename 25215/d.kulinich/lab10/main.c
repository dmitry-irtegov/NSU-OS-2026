#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    

    if (argc < 2) {
        fprintf(stderr, "Use: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {

        execvp(argv[1], argv + 1);
        

        perror("execvp can't launch program");
        
        exit(1);
    } else {

        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return 1;
        }


        if (WIFEXITED(status)) {
            // regular complete
            printf("Exit coode: %d\n", WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) {
            // signal killed
            printf("Kill by signal: %d\n", WTERMSIG(status));
        } 
        else {
            printf("Unknown exit status\n");
        }
    }

    return 0;
}