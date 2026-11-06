#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main() {

    pid_t id = fork();

    if(id < 0){
        perror("fork() err");
        exit(-1);
    } else if (id == 0) {
        execlp("cat", "cat", "longfile.txt", (char *)NULL);
    } else {
        int stat;
        pid_t closed_id = waitpid(id, &stat, 0);
        
        printf("\n");
        printf("PARENT is active, created 1 CHILD (PID = %d).", id);
        printf("\nclosing...");

        if(closed_id < 0) {
            perror("waitpid err");
            exit(-1);
        } 
        exit(0);
    }
        
    return 0;
}
