#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2){
        _exit(1);
    }
    pid_t pid = fork();
    if (pid == -1){
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);
        _exit(1);
    } 
    else {
        printf("\n parallel working\n");
        int status;
        waitpid(pid,&status,0);
        printf("\n fiel's cat: \n");
    }
    printf("\n Parent's text fineshed\n");
    return 0;
}