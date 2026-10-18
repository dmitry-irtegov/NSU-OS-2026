#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void) {

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        
        execlp("cat", "cat", "long_file.txt", NULL);
        
        perror("exec failed");
        return 1;
    } else {
        
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid failed");
            return 1;
        }

        printf("\nChild process complete work!\n");

    }

    return 0;
}