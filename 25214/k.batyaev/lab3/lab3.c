#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void open_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        printf("File '%s' successfully opened.\n", filename);
        fclose(file);
    } else {
        perror("Error opening file");
    }
}

int main() {
    const char *filename = "lab3.txt";

    printf("Real UID (RUID): %d\n", getuid());
    printf("Effective UID (EUID): %d\n", geteuid());

    open_file(filename);

    if (setuid(getuid()) < 0) {
        perror("Error setuid");
        return 1;
    }

    printf("Real UID (RUID): %d\n", getuid());
    printf("Effective UID (EUID): %d\n", geteuid());
    
    open_file(filename);

    return 0;
}
