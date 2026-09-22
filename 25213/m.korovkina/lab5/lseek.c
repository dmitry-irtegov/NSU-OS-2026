#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

typedef struct line_t {
    off_t offset;
    off_t length;
} line;

typedef struct arr_t{
    line* lines;
    int size;
    int capacity;
} arr;

int add_line_info(arr* lines_array, off_t offset, off_t length) {
    if (lines_array->size == lines_array->capacity) {
        lines_array->capacity *= 2;
        line* new_lines = realloc(lines_array->lines, lines_array->capacity * sizeof(line));
        if (new_lines == NULL) {
            perror("Realloc error");
            return 1;
        }
        lines_array->lines = new_lines;
    }
    lines_array->lines[lines_array->size].offset = offset;
    lines_array->lines[lines_array->size++].length = length;
    return 0;
}

void free_array(arr* lines_array, int fd) {
    free(lines_array->lines);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("No file name\n");
        return 1;
    }
    char* filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the file");
        return 1;
    }
    arr lines_array;
    lines_array.size = 0;
    lines_array.capacity = 2;
    lines_array.lines = malloc(lines_array.capacity * sizeof(line));
    if (lines_array.lines == NULL) {
        perror("Malloc error");
        close(fd);
        return 1;
    }
    char symbol;
    off_t cur_offset = 0;
    off_t cur_length = 0;
    ssize_t byte_count;
    while ((byte_count = read(fd, &symbol, 1)) == 1) {
        if (symbol == '\n') {
            if (add_line_info(&lines_array, cur_offset, cur_length) == 1) {
                free_array(&lines_array, fd);
                return 1;
            }
            cur_offset = lseek(fd, 0L, 1);
            cur_length = 0;
        }
        else {
            cur_length++;
        }
    }
    if (byte_count != 0) {
        perror("Failed to read the file");
        free_array(&lines_array, fd);
        return 1;
    }
    if (cur_length != 0) {
        if (add_line_info(&lines_array, cur_offset, cur_length) == 1) {
            free_array(&lines_array, fd);
            return 1;
        }
    }
    int line_index;
    while (1) {
        printf("Enter the number of the line you want to print\n");
        int res = scanf("%d", &line_index);
        if (res == 0) {
            while (1) {
                if (getchar() == '\n') {
                    break;
                }
            }
            continue;
        }
        if (line_index == 0) {
            break;
        }
        if (line_index < 1 || line_index > lines_array.size) {
            printf("There is no line with that number\n");
            continue;
        }
        off_t len = lines_array.lines[line_index - 1].length;
        char* buff = malloc((len + 1) * sizeof(char));
        if (buff == NULL) {
            perror("Malloc error");
            free_array(&lines_array, fd);
            return 1;
        }
        lseek(fd, lines_array.lines[line_index - 1].offset, 0);
        ssize_t bytes = read(fd, buff, len);
        if (bytes != len) {
            perror("Failed to read the line");
            free_array(&lines_array, fd);
            free(buff);
            return 1;
        }
        buff[len] = '\0';
        printf("%s\n", buff);
        free(buff);
    }
    free_array(&lines_array, fd);
    return 0;
}
