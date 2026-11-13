#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp;

    printf("Real UID: %d, Effective UID: %d\n", (int)getuid(), (int)geteuid());

    fp = fopen("test_file.txt", "r");
    if (fp == NULL) {
        perror("fopen failed (before setuid)");
    } else {
        printf("fopen succeeded.\n");
        fclose(fp);
    }

    if (setuid(getuid()) < 0) {
        perror("setuid failed");
        return 1;
    }

    printf("After setuid -> Real UID: %d, Effective UID: %d\n", (int)getuid(), (int)geteuid());

    fp = fopen("test_file.txt", "r");
    if (fp == NULL) {
        perror("fopen failed (after setuid)");
    } else {
        printf("fopen succeeded after setuid.\n");
        fclose(fp);
    }

    return 0;
}
