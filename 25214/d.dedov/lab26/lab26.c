#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    FILE* p = popen("./reader", "w");
    if (!p) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }
    char string[] = "SomeThing";
    if (fwrite(string, sizeof(char), strlen(string) + 1, p) != strlen(string) + 1) {
        perror("fwrite failed");
        exit(EXIT_FAILURE);
    }
    if (fflush(p) == EOF) {
        perror("fflush failed");
        exit(EXIT_FAILURE);
    }
    if (pclose(p) == -1) {
        perror("pclose failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}
