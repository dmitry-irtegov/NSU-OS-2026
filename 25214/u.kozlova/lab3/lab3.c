#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Печатает реальный и эффективный идентификаторы пользователя
void print_uids(void)
{
    printf("Real UID:      %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
}

// Пробует открыть файл и выводит ошибку в случае неудачи
void open_file(const char *name) 
{
    FILE* file = fopen(name, "r");
    if (file == NULL) 
    {
        perror("fopen");
    } 
    else 
    {
        fclose(file);
    }
}

int main() 
{
    print_uids();
    open_file("lab3.txt");

    if (setuid(getuid()) == -1) {
        perror("setuid");
    }

    print_uids();
    open_file("lab3.txt");

    return 0; 
}
