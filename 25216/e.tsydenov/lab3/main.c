#include <stdio.h>
#include <unistd.h>

void test_access(const char *filename) {
    printf("RUID: %d, EUID: %d\n", getuid(), geteuid());

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("error opening file");
        return;
    }

    fclose(f);
}

int main() {
    const char *filename = "file.txt";

    test_access(filename);

    int r = setuid(getuid());
    if (r == -1) {
        perror("setuid failed");
    }

    test_access(filename);
}
