#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>



int main() {
    pid_t pid = fork();


    if (pid == 0) {

        execlp("cat", "cat", "/usr/include/unistd.h", NULL);

    } else {

        int stat;
        wait(&stat);
        printf("\nВыполнен cat файла /usr/include/unistd.h\n");
    }

    return 0;
}