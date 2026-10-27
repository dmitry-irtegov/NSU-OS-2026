#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* Печать ruid и euid. */
static void print_ids(const char *label)
{
    printf("%s: uid=%ld, euid=%ld\n",
           label, (long)getuid(), (long)geteuid());
}

/* Попытка открыть файл через fopen/fclose. */
static void try_open(const char *path)
{
    FILE *f = fopen(path, "r+");
    if (f == NULL) {
        perror(path);
        return;
    }
    printf("fopen(\"%s\") succeeded\n", path);
    fclose(f);
}

int main(void)
{
    /* Шаг 1: напечатать идентификаторы и попробовать fopen. */
    print_ids("before setuid");
    try_open("file");

    /* Шаг 2: сделать euid == ruid. */
    setuid(getuid());

    /* Шаг 3: повторить то же самое. */
    print_ids("after setuid");
    try_open("file");

    return EXIT_SUCCESS;
}