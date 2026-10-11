#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void print_uids() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID %d\n", geteuid());
}

void check_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
    } 
    else {
        printf("File opened successfully\n");
        fclose(file);
    }
}

int main() {
    const char* file = "text.txt";

    printf("Before setuid:\n");
    print_uids();
    check_file(file);

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    printf("After setuid:\n");
    print_uids();
    check_file(file);

    return 0;
}