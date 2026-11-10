#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("You should give me a file to cat\n");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid;

    child_pid = fork();

    switch (child_pid)
    {
    case -1:
        perror("fork() caused an error\n");
        exit(EXIT_FAILURE);
    case 0:
        printf("It's the child process - (%d)\n", getpid());
        execl("/bin/cat", "cat", argv[1], NULL);
        perror("execl() caused an error\n");
        exit(EXIT_FAILURE);
    default:
        printf("It's the parent process - (%d)\n", getpid());
        int wstatus;
        if (waitpid(child_pid, &wstatus, 0) == -1) {
            perror("waitpid() caused an error\n");
            exit(EXIT_FAILURE);
        }
        printf("The child - (%d) - has been terminated with code %d\n", child_pid, wstatus);
        break;
    }

    exit(EXIT_SUCCESS);

}