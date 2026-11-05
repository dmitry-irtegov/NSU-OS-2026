#include <stdio.h>

extern char **environ;

int main(int argc, char* argv[]) {
    printf("argc = %d\n", argc);
    int i;
    for (i = 0; i < argc; i++) {
        printf("  argc[%d] = %s\n", i, argv[i]);
    }
    char **p = environ;
    printf("environ:\n");
    while (*p != NULL) {
        printf("  %s\n", *p);
        p++;
    }
    return 0;
}