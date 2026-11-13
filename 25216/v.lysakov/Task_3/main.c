#include <stdio.h>
#include <unistd.h>

void print_ids(void)
{
    printf("Real UID:      %ld\n", (long)getuid());
    printf("Effective UID: %ld\n", (long)geteuid());
}

void try_open_file(const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("fopen");
        return;
    }

    printf("File opened successfully\n");

    fclose(file);
}

int main(void)
{
    const char *filename = "data.txt";

    printf("Before setuid:\n");
    print_ids();
    try_open_file(filename);

    printf("\nCalling setuid(getuid())...\n");

    if (setuid(getuid()) == -1) {
        perror("setuid");
        return 1;
    }

    printf("\nAfter setuid:\n");
    print_ids();
    try_open_file(filename);

    return 0;
}