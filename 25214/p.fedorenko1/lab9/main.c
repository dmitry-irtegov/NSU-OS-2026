#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        execlp("cat", "cat", "file.txt", NULL);
        perror("execlp");
        return 1;
        
    } else if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (waitpid(pid, NULL, 0) == -1) { 
        perror("waitpid");
        return 1;
    }
    
    printf("\nparent\n");

    return 0;
}
