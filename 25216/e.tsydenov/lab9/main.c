#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    execlp("cat", "cat", "file.txt", (char *)NULL);
    perror("cat");
    _exit(1);
  }

  printf("Parent process\n");

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    return 1;
  }

  if (WIFEXITED(status)) {
    printf("Child exited with code: %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Child terminated by signal: %d\n", WTERMSIG(status));
  }

  printf("Child process finished\n");
  return 0;
}
