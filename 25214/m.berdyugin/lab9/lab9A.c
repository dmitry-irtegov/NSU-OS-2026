cat > lab-9A.c << 'EOF'
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


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
    }

    return 0;
}
EOF