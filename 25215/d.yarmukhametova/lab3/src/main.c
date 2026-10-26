#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void print_uid() {
    printf("real uid: %d\n", getuid());
    printf("effective uid: %d\n", geteuid());
}

int main() {
    printf("before changing uids\n");
    print_uid();

    FILE *file;
    file = fopen("file", "r");

    if (file == NULL) {
        perror("error when opening a file");
    } else {
        fclose(file); 
    }

    int flag = setuid(getuid());
    if (flag == -1) {
        perror("error executing setuid");
        return 1;
    }

    printf("after changing uid\n");
    print_uid();

    file = fopen("file", "r");

    if (file == NULL) {
        perror("error when opening a file");
    } else {
        fclose(file); 
    }
          

    return 0;
}