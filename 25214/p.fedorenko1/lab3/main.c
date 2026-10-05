#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void print_uids(void) {
    printf("real user id: %d; effective user id: %d\n", getuid(), geteuid());
}

void try_open_file(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    printf("success open file\n");

    if (fclose(file) == EOF) {
        perror("fclose");
    }
}

int main(void) {
    uid_t uid;

    print_uids();
    try_open_file("data.txt");

    uid = getuid();

    if (setuid(uid) == -1) {
        perror("setuid");
        return 1;
    }

    print_uids();
    try_open_file("data.txt");

    return 0;
}
