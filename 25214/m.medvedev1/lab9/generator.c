#include <stdio.h>

#define N 10000

int main() {
    FILE *file = fopen("text.txt", "w");

    for (int i = 1; i <= N; i++) {
        fprintf(file, "%d\n", i);
    }

    fclose(file);

    return 0;
}