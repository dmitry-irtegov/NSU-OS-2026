#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    pid_t pid;
    pid = fork();

    if (pid < 0){
        perror("fork");
        exit(1);
    }

    if (pid == 0){
        execlp("cat", "cat", "long_file.txt", NULL);
        perror("execlp");
        exit(1);
    } 
    else {
        printf("Parent: Child process is running (pid=%d)\n", pid);
        fflush(stdout);
        wait(NULL);
        printf("Parent: subprocess completed\n");
    }

    return 0;
}