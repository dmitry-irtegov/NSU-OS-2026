cat > lab-9B.c << 'EOF'
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", "longfile.txt", NULL);
        perror("execlp");
        _exit(1);
    } else {
        printf("This text is printed by the parent process\n");
        fflush(stdout);

        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
        }

        printf("The child process has finished\n");
    }

    return 0;
}
EOF