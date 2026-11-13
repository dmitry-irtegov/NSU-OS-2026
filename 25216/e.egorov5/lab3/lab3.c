#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Не указан .txt файл для проверки файла на права доступа %s\n", argv[0]);
        return 1;
    }

    FILE *f;

    printf("Real UID: %d, Effective UID: %d\n", (int)getuid(), (int)geteuid());

    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("Ошибка открытия файла (попытка 1)");
    } else {
        printf("Файл успешно открыт!\n");
        fclose(f);
    }

    setuid(getuid());

    printf("\nПопытался присвоить Реальному UID значение Эффективного UID\n");
    printf("Real UID: %d, Effective UID: %d\n", (int)getuid(), (int)geteuid());

    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("Ошибка открытия файла (попытка 2)");
    } else {
        printf("Файл успешно открыт!\n");
        fclose(f);
    }

    return 0;
}
