cat > lab-11.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **saved;

    saved = environ;
    environ = (char **) envp;

    execvp(file, argv);

    environ = saved;
    return -1;
}


int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        char *args[] = {"cat", "data.txt", NULL};
        char *newenv[] = {"PATH=/bin:/usr/bin", NULL};

        execvpe("cat", args, newenv);
        perror("execvpe");
        _exit(1);
    }

    waitpid(pid, NULL, 0);
    printf("Child finished\n");

    return 0;
}
EOF