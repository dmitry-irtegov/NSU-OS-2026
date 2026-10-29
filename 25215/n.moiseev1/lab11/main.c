#define __EXTENSIONS__ 1
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

int execvpe(const char *file, char *argv[], char *envp[]) {

	char **old_env = environ;
	environ = envp;
	int result = execvp(file, argv);
	environ = old_env;

	return result;
}

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
		char **new_env = calloc(1, sizeof(*new_env));
		if (new_env == NULL) {
        		perror("calloc");
        		return 1;
		}

		char *line = NULL;
		size_t capacity = 0;
		size_t count = 0;
		ssize_t length;

		printf("Enter NAME=VALUE; an empty line finishes input:\n");
		fflush(stdout);

		while ((length = getline(&line, &capacity, stdin)) != -1) {
        		if (length > 0 && line[length - 1] == '\n') {
                		line[--length] = '\0';
        		}

        		if (length == 0) {
                		break;
        		}

        		char **tmp = realloc(new_env, (count + 2) * sizeof(*new_env));
        		if (tmp == NULL) {
                		perror("realloc");
                		return 1;
        		}

        		new_env = tmp;
        		new_env[count++] = line;
        		new_env[count] = NULL;

        		line = NULL;
        		capacity = 0;
		}

		if (length == -1 && !feof(stdin)) {
        		perror("getline");
        		return 1;
		}
		free(line);

		execvpe(args[1], &args[1], new_env);
		perror("Error after calling execvpe");

		for (size_t i = 0; i < count; i++) {
        		free(new_env[i]);
		}
		free(new_env);

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
