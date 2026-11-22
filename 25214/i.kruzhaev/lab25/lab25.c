#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#define MSGSIZE 40


int main() {
    int fildes[2];
    pid_t pid;

    if (pipe(fildes) == -1) {
        perror("Pipe open failed");
        return 1;
    }

    if ((pid = fork()) > 0) {
        close(fildes[0]);

        static char msg_out[MSGSIZE] = "Text in lower and UPPER reGIstOR\n";
        write(fildes[1], msg_out, MSGSIZE);

        close(fildes[1]);
    }
    else if (pid == 0) {
        close(fildes[1]);
        static char msg_in[MSGSIZE];
        read(fildes[0], msg_in, MSGSIZE);

        for (int i = 0; msg_in[i] != '\0'; i++) {
            printf("%c", toupper(msg_in[i]));
        }

        close(fildes[0]);
    }
    else {
        perror("Fork failed");
        return 2;
    }

    return 0;
}