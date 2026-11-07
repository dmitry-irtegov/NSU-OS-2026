#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define INITIAL_CAPACITY 100

typedef struct {
    off_t offset;
    size_t length;
} line_t;

int main(int argc, char *argv[]) {
    int fd;
    size_t capacity;
    size_t line_count;
    line_t *lines;
    char buf;
    size_t current_len;
    off_t cur_pos;
    line_t *temp;
    size_t i;
    int line_num;
    char read_buf[1024];
    size_t idx;
    size_t to_read;
    size_t total_read;
    size_t chunk;
    ssize_t r;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    capacity = INITIAL_CAPACITY;
    line_count = 0;
    lines = (line_t *)malloc(capacity * sizeof(line_t));
    if (!lines) {
        perror("Memory allocation error");
        close(fd);
        return 1;
    }

    lines[0].offset = 0;
    line_count = 1;
    current_len = 0;

    while (read(fd, &buf, 1) > 0) {
        current_len++;
        if (buf == '\n') {
            lines[line_count - 1].length = current_len;

            if (line_count >= capacity) {
                capacity *= 2;
                temp = (line_t *)realloc(lines, capacity * sizeof(line_t));
                if (!temp) {
                    perror("Memory reallocation error");
                    free(lines);
                    close(fd);
                    return 1;
                }
                lines = temp;
            }

            cur_pos = lseek(fd, 0L, SEEK_CUR);
            lines[line_count].offset = cur_pos;
            lines[line_count].length = 0;
            line_count++;
            current_len = 0;
        }
    }

    if (current_len > 0) {
        lines[line_count - 1].length = current_len;
    } else if (line_count > 1 && lines[line_count - 1].offset == lseek(fd, 0L, SEEK_CUR)) {
        line_count--;
    }

    printf("\n+----------+------------------+------------------+\n");
    printf("| Line #   | Offset (bytes)   | Length (bytes)   |\n");
    printf("+----------+------------------+------------------+\n");
    for (i = 0; i < line_count; i++) {
        printf("| %8lu | %16ld | %16lu |\n", 
               (unsigned long)(i + 1), 
               (long)lines[i].offset, 
               (unsigned long)lines[i].length);
    }
    printf("+----------+------------------+------------------+\n");
    printf("Total lines: %lu\n\n", (unsigned long)line_count);

    while (1) {
        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        if (scanf("%d", &line_num) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        if (line_num == 0) {
            break;
        }

        if (line_num < 1 || (size_t)line_num > line_count) {
            printf("Error: invalid line number. Enter a number between 1 and %lu.\n", (unsigned long)line_count);
            continue;
        }

        idx = line_num - 1;

        if (lseek(fd, lines[idx].offset, SEEK_SET) < 0) {
            perror("lseek error");
            continue;
        }

        printf("Content of line %d:\n", line_num);
        fflush(stdout);

        to_read = lines[idx].length;
        total_read = 0;

        while (total_read < to_read) {
            chunk = to_read - total_read;
            if (chunk > sizeof(read_buf)) {
                chunk = sizeof(read_buf);
            }
            r = read(fd, read_buf, chunk);
            if (r <= 0) {
                break;
            }
            write(STDOUT_FILENO, read_buf, r);
            total_read += r;
        }
        
        if (to_read == 0 || read_buf[to_read - 1] != '\n') {
            write(STDOUT_FILENO, "\n", 1);
        }
    }

    free(lines);
    close(fd);
    return 0;
}