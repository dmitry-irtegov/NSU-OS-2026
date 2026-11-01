#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA_FILE "data.txt"

int main(void)
{
    FILE *f;

    printf("real uid=%d, effective uid=%d\n", (int)getuid(), (int)geteuid());

    f = fopen(DATA_FILE, "r+");
    if (f != NULL) {
        printf("fopen: ok\n");
        fclose(f);
    } else {
        perror("fopen");
    }

    if (setuid(getuid()) == -1) {
        perror("setuid");
    }

    printf("real uid=%d, effective uid=%d\n", (int)getuid(), (int)geteuid());

    f = fopen(DATA_FILE, "r+");
    if (f != NULL) {
        printf("fopen: ok\n");
        fclose(f);
    } else {
        perror("fopen");
    }

    return 0;
}