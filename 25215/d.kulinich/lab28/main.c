#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h> 
#include <unistd.h>

int main(void) {
    FILE *fp[2];
    
    srand((unsigned int)time(NULL));

    if (p2open("sort -n", fp) != 0) {
        perror("p2open failed");
        return 1;
    }

    for (int i = 0; i < 100; i++)
        fprintf(fp[0], "%d\n", rand() % 100);

    fclose(fp[0]);

    printf("--- Sorted nums ---\n");

    int num;
    int count = 0;
    while (fscanf(fp[1], "%d", &num) == 1) {
        printf("%2d ", num);
        count++;
        if (count % 10 == 0) {
            printf("\n");
        }
    }

    if (count % 10 != 0) {
        printf("\n");
    }

    p2close(fp);

    return 0;
}