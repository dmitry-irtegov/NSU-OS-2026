#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static void check_file(const char *filename) {
    FILE *file;

    printf("Real UID: %lu\n", (unsigned long)getuid());
    printf("Effective UID: %lu\n", (unsigned long)geteuid());
    fflush(stdout);

    file = fopen(filename, "r+");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    printf("File opened successfully\n");

    if (fclose(file) == EOF) {
        perror("fclose");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    printf("Before setuid:\n");
    check_file(argv[1]);

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    printf("\nAfter setuid:\n");
    check_file(argv[1]);

    return 0;
}