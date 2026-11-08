#include <stdio.h>
#include <stdlib.h>
#define MAX_MSG_SIZE 50
int main() {
    static char text[] = "This is another task!";
    static char command[MAX_MSG_SIZE];
    static char buffer[MAX_MSG_SIZE];
    sprintf(command, "echo \"%s\"", text);
    FILE* fpin = popen(command, "r");
    if (fpin == NULL) {
        perror("popen read");
        exit(1);
    }
    FILE* fpout = popen("tr \'[:lower:]\' \'[:upper:]\'", "w");
    if (fpout == NULL) {
        perror("popen write");
        pclose(fpin);
        exit(2);
    }
    int exit_code = 0;
    while (fgets(buffer, MAX_MSG_SIZE, fpin) != NULL) {
        if (fprintf(fpout, "%s", buffer) < 0) {
            perror("fprintf");
            if (exit_code == 0) {
                exit_code = 3;
            }
            break;
        }
    }
    if (ferror(fpin)) {
        perror("fgets");
        if (exit_code == 0) {
            exit_code = 4;
        }
    }
    int status_in = pclose(fpin);
    if (status_in == -1) {
        perror("pclose echo");
        if (exit_code == 0) {
            exit_code = 5;
        }
    } else if (!WIFEXITED(status_in) || WEXITSTATUS(status_in) != 0) {
        fprintf(stderr, "echo error\n");
        if (exit_code == 0) {
            exit_code = 5;
        }
    }
    int status_out = pclose(fpout);
    if (status_out == -1) {
        perror("pclose tr");
        if (exit_code == 0) {
            exit_code = 6;
        }
    } else if (!WIFEXITED(status_out) || WEXITSTATUS(status_out) != 0) {
        fprintf(stderr, "tr error\n");
        if (exit_code == 0) {
            exit_code = 6;
        }
    }
    return exit_code;
}