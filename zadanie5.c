#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INITIAL_CAPACITY 100
#define READ_BUF_SIZE    4096
#define INPUT_BUF_SIZE   64

typedef struct {
    off_t offset;
    off_t length;
} line_t;

static ssize_t read_input_line(char *buf, size_t size)
{
    size_t len = 0;
    int overflow = 0;

    for (;;) {
        char c;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) {
            if (len == 0 && !overflow) return 0;
            break;
        }
        if (c == '\n') break;
        if (len + 1 < size) {
            buf[len++] = c;
        } else {
            overflow = 1;
        }
    }
    buf[len] = '\0';
    return (ssize_t)len;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(stderr, "fstat: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "%s: not a regular file\n", argv[1]);
        close(fd);
        return 1;
    }

    size_t capacity = INITIAL_CAPACITY;
    size_t line_count = 0;
    line_t *lines = malloc(capacity * sizeof *lines);
    if (lines == NULL) {
        fprintf(stderr, "malloc: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    off_t pos = 0;
    off_t cur_start = 0;
    off_t cur_len = 0;
    char buf[READ_BUF_SIZE];
    ssize_t n;

    while ((n = read(fd, buf, sizeof buf)) > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            ++cur_len;
            ++pos;
            if (buf[i] == '\n') {
                if (line_count >= capacity) {
                    capacity *= 2;
                    line_t *temp = realloc(lines, capacity * sizeof *lines);
                    if (temp == NULL) {
                        fprintf(stderr, "realloc: %s\n", strerror(errno));
                        free(lines);
                        close(fd);
                        return 1;
                    }
                    lines = temp;
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
        fprintf(stderr, "read: %s\n", strerror(errno));
        free(lines);
        close(fd);
        return 1;
    }

    if (cur_len > 0) {
        if (line_count >= capacity) {
            capacity *= 2;
            line_t *temp = realloc(lines, capacity * sizeof *lines);
            if (temp == NULL) {
                fprintf(stderr, "realloc: %s\n", strerror(errno));
                free(lines);
                close(fd);
                return 1;
            }
            lines = temp;
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

    for (;;) {
        char input_buf[INPUT_BUF_SIZE];

        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        ssize_t r = read_input_line(input_buf, sizeof input_buf);
        if (r < 0) {
            fprintf(stderr, "read stdin: %s\n", strerror(errno));
            break;
        }
        if (r == 0) {
            fprintf(stderr, "\nEOF on input, exiting\n");
            break;
        }

        char *end;
        errno = 0;
        long val = strtol(input_buf, &end, 10);

        if (end == input_buf || errno != 0) {
            fprintf(stderr, "Invalid input: not a number\n");
            continue;
        }
        if (*end != '\0') {
            fprintf(stderr, "Invalid input: trailing garbage\n");
            continue;
        }

        if (val == 0) {
            break;
        }

        if (val < 0 || (unsigned long)val > (unsigned long)line_count) {
            fprintf(stderr,
                    "Error: line number must be in range 1..%zu\n",
                    line_count);
            continue;
        }

        if (lseek(fd, lines[(size_t)val - 1].offset, SEEK_SET) < 0) {
            fprintf(stderr, "lseek: %s\n", strerror(errno));
            continue;
        }

        printf("Content of line %ld:\n", val);
        fflush(stdout);

        {
            off_t to_read = lines[(size_t)val - 1].length;
            off_t total_read = 0;
            char last = '\n';

            while (total_read < to_read) {
                size_t chunk = (to_read - total_read > (off_t)sizeof buf)
                                   ? sizeof buf
                                   : (size_t)(to_read - total_read);
                ssize_t rr = read(fd, buf, chunk);
                if (rr < 0) {
                    if (errno == EINTR) continue;
                    fprintf(stderr, "read: %s\n", strerror(errno));
                    break;
                }
                if (rr == 0) {
                    fprintf(stderr, "warning: unexpected EOF\n");
                    break;
                }

                last = buf[rr - 1];

                if (write(STDOUT_FILENO, buf, (size_t)rr) != rr) {
                    fprintf(stderr, "write: %s\n", strerror(errno));
                    break;
                }
                total_read += rr;
            }

            if (last != '\n') {
                if (write(STDOUT_FILENO, "\n", 1) != 1) {
                    fprintf(stderr, "write: %s\n", strerror(errno));
                }
            }
        }
    }

    free(lines);
    close(fd);
    return 0;
}