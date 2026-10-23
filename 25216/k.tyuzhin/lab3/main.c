#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void func(const char* path) {
    printf("uid = %ld; euid = %ld\n", (long)getuid(),  (long)geteuid());

    FILE* file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    printf("File opened successfully.\n");
    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    const char* filename = argv[1];
    
    func(filename);

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    printf("After setuid(getuid()).\n");

    func(filename);

    return 0;
}