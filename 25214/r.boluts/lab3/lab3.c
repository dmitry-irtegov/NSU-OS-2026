#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Real user's ID: %lu\n", (unsigned long) getuid());
    printf("Effective user's ID: %lu\n", (unsigned long) geteuid());

    FILE* f = fopen("file.txt", "r");
    if (f == NULL) {
        perror("fopen");
    } else {
        fclose(f);
    }

    if (setuid(getuid()) == -1) {
        perror("setuid");
    }

    printf("Real user's ID: %lu\n", (unsigned long) getuid());
    printf("Effective user's ID: %lu\n", (unsigned long) geteuid());

    f = fopen("file.txt", "r");
    if (f == NULL) {
        perror("fopen");
    } else {
        fclose(f);
    }
    return 0;
}