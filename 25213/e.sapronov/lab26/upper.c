#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int c;
    while ((c = getchar()) != EOF) {
        if (putchar(toupper((unsigned char)c)) == EOF) {
            perror("putchar");
            exit(1);
        }
    }

    return 0;
}
