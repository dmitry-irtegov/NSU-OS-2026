#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int code = 0;

    if (argc > 1) {
        code = atoi(argv[1]);
    }

    printf("Returning %d\n", code);
    return code;
}
