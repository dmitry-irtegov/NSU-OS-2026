#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* args[]) {

	if (argc < 2) {
        	printf("Usage: %s command [arguments...]\n", args[0]);
        	return 1;
	}

        pid_t pid = fork();

        if (pid == -1) {
                perror("Error when creating a child process");
                return 1;
        } else if (pid == 0) {
                execvp(args[1], &args[1]);
                perror("Error after calling execvp");
                return 1;
        } else {
		int status;

		if (waitpid(pid, &status, 0) == -1) {
		        perror("waitpid");
		        return 1;
		}

		if (WIFEXITED(status)) {
		        printf("The child exit code: %d\n", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
		        printf("The child terminated by signal: %d\n", WTERMSIG(status));
		}
	}

        return 0;
}
