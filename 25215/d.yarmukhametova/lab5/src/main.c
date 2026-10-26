#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

typedef struct table {
    off_t offset;
    off_t len;
} offset_table;

void initTable(size_t* cap, size_t* count, offset_table** table) {
    *cap = 100;
    *count = 0;
    *table = (offset_table*)malloc(sizeof(offset_table) * (*cap));
    if (*table == NULL) {
        perror("malloc");
        exit(1);
    }
}

int addLine(size_t* cap, size_t* count, offset_table** table, off_t len, off_t offset) {
    if (*count >= *cap) {
        size_t new_cap = (*cap == 0) ? 8 : *cap * 2;
        offset_table* temp = realloc(*table, new_cap * sizeof(offset_table));
        if (temp == NULL) {
            perror("realloc");
            return -1;
        }
        *table = temp;
        *cap = new_cap;
    } 
    (*table)[*count].offset = offset;
    (*table)[*count].len = len;
    (*count)++;
    return 1;
}

int createTable(int fd, size_t* capacity, size_t* count, offset_table** table) {
    char buf[4096];
    ssize_t bytes_read;
    off_t current_offset = 0;
    off_t line_start = 0;
    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') {
                off_t line_end = current_offset + i;
                off_t len = line_end - line_start +1;
                if (addLine(capacity, count, table, len, line_start) == -1) {
                    fprintf(stderr, "error when added line in offset table, line: %ld\n", *count);
                    return -1;
                }
                line_start = line_end + 1;
            }
        }
        current_offset += bytes_read;
    }
    if (bytes_read == -1) {
        perror("read");
        return -1;
    }
    off_t end_pos = lseek(fd, 0L, SEEK_CUR);
    if (end_pos > line_start) {
        off_t len = end_pos - line_start;
        if (addLine(capacity, count, table, len, line_start) == -1) {
            fprintf(stderr, "error when added line in offset table, line: %ld\n", *count);
            return -1;
        }
    }
    return 1;
}

void console(size_t count, offset_table* table, int fd) {
    char *string = NULL;
    size_t size = 0;
    ssize_t chars_read;
    long line_number;
    if (count == 0) {
        printf("Файл пуст.\n");
        return;
    }
    while(1) {
        printf("Введите номер строки (от 1 до %zu), 0 - для выхода:\n", count);
        chars_read = getline(&string, &size, stdin);
        
        if (chars_read < 0) {
            perror("getline");
            free(string);
            return;
        }

        if (chars_read == 1 && string[0] == '\n') {
            continue;
        }

        char *endptr;
        errno = 0;
        line_number = strtol(string, &endptr, 10);
        if (errno == ERANGE) {
            fprintf(stderr, "Число слишком большое.\n");
            continue;
        }

        if (endptr == string || (*endptr != '\n' && *endptr != '\0')) {
            fprintf(stderr, "Введите корректное число (только цифры).\n");
            continue;
        }

        if (line_number == 0){
            printf("Выход из программы.\n");
            break;
        }

        if ((line_number-1 >= count) || (line_number < 0)) {
            fprintf(stderr, "Нет такой строки\n");
            continue;
        }

        long line_idx = line_number-1;
        off_t current_offset = table[line_idx].offset;
        off_t current_len = table[line_idx].len;

        if (lseek(fd, current_offset, SEEK_SET) == -1) {
            perror("lseek");
            break;
        }
        char* buffer = (char*)malloc(sizeof(char) * (current_len + 1));
        ssize_t number_of_bytes = read(fd, buffer, current_len);
        if (number_of_bytes == -1) {
            perror("read");
            free(buffer);
            break;
        }
        buffer[number_of_bytes] = '\0';
        printf("Строка %ld: %s\n", line_number, buffer); 

        free(buffer);
    }
    if (string != NULL) {
        free(string);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    size_t capacity;
    size_t count;
    offset_table* table = NULL;

    initTable(&capacity, &count, &table);
    
    if (createTable(fd, &capacity, &count, &table) == -1) {
        fprintf(stderr, "error when create offset table\n");
        close(fd);
        free(table);
        return 1;
    }

    console(count, table, fd);

    if (close(fd) == -1) {
        perror("close");
    }
    free(table);

    return 0;
}
