#include <sys/types.h>
#include<sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[ ]) {
    char ch, first, last;
        pid_t pid;
    int i;

    if ((pid = fork()) > 0) {
        // if (waitpid(pid, NULL, 0) == -1) {
        //     perror("child process failed");
        //     exit(1);
        // }

        printf("\n Какой-либо текст.\n");
    } else if (pid == 0) {
        execl("/bin/cat", "cat", "pushkin-skazki.txt", (char*) 0);

        perror("exec cat failed");
        exit(1);
    } else {
        perror(argv[0]);
        exit(1);
    }

    return 0;
}