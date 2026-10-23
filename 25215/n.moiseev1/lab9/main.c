#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

	pid_t pid = fork();

	if (pid == -1) {
		perror("Error when creating a child process");
		return 1;
	} else if (pid == 0) {
		char* args[] = {"cat", "file.txt", NULL};
		execvp("cat", args);
		perror("Error after calling execvp");
		return 1;
	} else {
		waitpid(pid, NULL, 0);
		printf("The parent writing something...\n");
	}

	return 0;
}
