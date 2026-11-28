#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define INITIAL_CAPACITY 100
#define READ_BUF_SIZE    4096

typedef struct {
    off_t offset;
    off_t length;
} line_t;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    size_t capacity = INITIAL_CAPACITY;
    size_t line_count = 0;
    line_t *lines = malloc(capacity * sizeof *lines);
    if (!lines) {
        perror("malloc");
        close(fd);
        return 1;
    }

    off_t pos = 0, cur_start = 0, cur_len = 0;
    char buf[READ_BUF_SIZE];
    ssize_t n;

    while ((n = read(fd, buf, sizeof buf)) > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            ++cur_len;
            ++pos;
            if (buf[i] == '\n') {
                if (line_count >= capacity) {
                    capacity *= 2;
                    lines = realloc(lines, capacity * sizeof *lines);
                    if (!lines) {
                        perror("realloc");
                        close(fd);
                        return 1;
                    }
                }
                lines[line_count].offset = cur_start;
                lines[line_count].length = cur_len;
                ++line_count;
                cur_start = pos;
                cur_len = 0;
            }
        }
    }
    if (n < 0) {
        perror("read");
        free(lines);
        close(fd);
        return 1;
    }
    if (cur_len > 0) {
        if (line_count >= capacity) {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof *lines);
            if (!lines) {
                perror("realloc");
                close(fd);
                return 1;
            }
        }
        lines[line_count].offset = cur_start;
        lines[line_count].length = cur_len;
        ++line_count;
    }

    printf("\n+----------+------------------+------------------+\n");
    printf("| Line #   | Offset (bytes)   | Length (bytes)   |\n");
    printf("+----------+------------------+------------------+\n");
    for (size_t i = 0; i < line_count; ++i) {
        printf("| %8zu | %16lld | %16lld |\n",
               i + 1,
               (long long)lines[i].offset,
               (long long)lines[i].length);
    }
    printf("+----------+------------------+------------------+\n");
    printf("Total lines: %zu\n\n", line_count);
    fflush(stdout);

    int line_index = 0;

    while (1) {
        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        int res = scanf("%d", &line_index);
        
        if (res == -1) {
            break;
        }
        
        if (res == 0) {
            scanf("%*s\n");
            continue;
        }

        if (line_index == 0) {
            break;
        }
        
        if (line_index < 1 || (size_t)line_index > line_count) {
            fprintf(stderr, "Error: line number must be in range 1..%zu\n", line_count);
            int c;
            while ((c = getchar()) != '\n' && c != EOF); 
            continue;
        }

        size_t idx = (size_t)line_index - 1;
        if (lseek(fd, lines[idx].offset, SEEK_SET) < 0) {
            perror("lseek");
            continue;
        }

        printf("Content of line %d:\n", line_index);
        
        off_t len = lines[idx].length;
        while (len > 0) {
            size_t chunk = (len > (off_t)sizeof buf) ? sizeof buf : (size_t)len;
            n = read(fd, buf, chunk);
            if (n == -1) {
                perror("read");
                free(lines);
                close(fd);
                return 1;
            }
            if (n == 0) {
                break;
            }
            fwrite(buf, 1, (size_t)n, stdout);
            len -= n;
        }
        fputc('\n', stdout);
    }

    free(lines);
    close(fd);
    return 0;
}