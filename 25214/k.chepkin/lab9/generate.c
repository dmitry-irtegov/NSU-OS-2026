#include <stdio.h>

#define N 10000

int main() {
    FILE* file = fopen("file.txt", "wt");
    for (int j = 0; j < N; j++) {
        fprintf(file, "{%d}", j);
    }
    fprintf(file, "\n");
}