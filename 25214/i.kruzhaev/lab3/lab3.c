#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func() {
    printf("Real User ID: %d.\n", getuid());
    printf("Effective User ID: %d.\n", geteuid());

    FILE* file = fopen("file", "r");

    if (file == NULL) {
        perror("Can't open file");
        exit(-1);
    }

    printf("File opened successful!\n\n");

    fclose(file);
}

int main() {
    func();

    setuid(getuid());

    func();

    return 0;
}