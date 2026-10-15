#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    uid_t ruid, euid;
    FILE *f;

    ruid = getuid();
    euid = geteuid();
    printf("1) RUID = %d, EUID = %d\n", ruid, euid);

    f = fopen("testfile.txt", "r");
    if (f == NULL)
    {
        perror("   fopen error (1)");
    }
    else
    {
        printf("   Файл успешно открыт!\n");
        fclose(f);
    }

    // Сбрасываем EUID до RUID
    if (setuid(ruid) < 0)
    {
        perror("setuid error");
        return 1;
    }
    printf("\n--> Выполнен setuid(%d)\n\n", ruid);

    ruid = getuid();
    euid = geteuid();
    printf("2) RUID = %d, EUID = %d\n", ruid, euid);

    f = fopen("testfile.txt", "r");
    if (f == NULL)
    {
        perror("   fopen error (2)");
    }
    else
    {
        printf("   Файл успешно открыт!\n");
        fclose(f);
    }

    return 0;
}