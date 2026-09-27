#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void) {

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        
        execlp("cat", "cat", "long_file.txt", NULL);
        
        perror("exec failed");
        exit(1);
    } else {
        int status;
        
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(1);
        }

        printf("\nChild process complete work!\n");

    }

    return 0;
}