#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

void print_uids(void){
    printf("Real UID (RUID)     : %d\n", getuid());
    printf("Effective UID (EUID): %d\n", geteuid()); 
}

void try_open_file(const char *filename){
    printf("Попытка открыть файл %s \n", filename);
    FILE *f = fopen(filename, "r"); 

    if (f == NULL){
        perror("Ошибка fopen"); 
    }else{
        printf("Файл успешно открыт\n");
        if(fclose(f) != 0){
            perror("Ошибка fclose");
        }
    }
}

int main(int argc, char *argv[]){

    if (argc < 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    
    printf("начальное состояние\n"); 
    print_uids();
    try_open_file(filename);

    printf("уравниваем ruid и euid\n");
    if(setuid(getuid()) == -1){
        perror("Ошибка выполнения setuid");
        exit(EXIT_FAILURE);
    }
    printf("Вызов setuid(getuid()) выполнен\n");

    printf("Повторная проверка\n");
    print_uids();
    try_open_file(filename); 

    return 0; 
}
