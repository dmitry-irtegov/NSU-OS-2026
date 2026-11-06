#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int fd;

void timeout_handler(int sig)
{
    char buf[1024];
    ssize_t bytes_read;

    printf("\nTime is over! File contents:\n");

    lseek(fd, 0, SEEK_SET);

    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, bytes_read);
    }

    close(fd);
    exit(0);
}

int main(int argc, char **argv)
{
    char c;
    int n;
    int lines = 0;
    int capacity = 16;

    if (argc < 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    long *offset = malloc(capacity * sizeof(long));
    long *length = malloc(capacity * sizeof(long));

    long start = 0;
    long pos;

    if (offset == NULL || length == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        printf("Error opening file\n");
        exit(1);
    }

    offset[0] = 0;

    char buf[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') {
                pos = lseek(fd, 0L, SEEK_CUR)
                    - (bytes_read - i - 1);

                if (lines == capacity) {
                    capacity *= 2;

                    long *new_offset =
                        realloc(offset, capacity * sizeof(long));
                    long *new_length =
                        realloc(length, capacity * sizeof(long));

                    if (new_offset == NULL || new_length == NULL) {
                        printf("Memory allocation error\n");
                        exit(1);
                    }

                    offset = new_offset;
                    length = new_length;
                }

                length[lines] = pos - start - 1;
                lines++;
                offset[lines] = pos;
                start = pos;
            }
        }
    }

    pos = lseek(fd, 0L, SEEK_CUR);

    if (pos > start) {
        if (lines == capacity) {
            capacity *= 2;

            long *new_offset =
                realloc(offset, capacity * sizeof(long));
            long *new_length =
                realloc(length, capacity * sizeof(long));

            if (new_offset == NULL || new_length == NULL) {
                printf("Memory allocation error\n");
                exit(1);
            }

            offset = new_offset;
            length = new_length;
        }

        length[lines] = pos - start;
        lines++;
    }

    printf("Line\tOffset\tLength\n");

    for (n = 0; n < lines; n++)
        printf("%d\t%ld\t%ld\n", n + 1, offset[n], length[n]);

    signal(SIGALRM, timeout_handler);

    while (1) {
        printf("Enter line number (0 - exit): ");

        alarm(5);

        int result = scanf("%d", &n);

        alarm(0);

        if (result != 1 || n == 0)
            break;

        if (n < 1 || n > lines) {
            printf("No such line\n");
            continue;
        }

        lseek(fd, offset[n - 1], SEEK_SET);

        long len = length[n - 1];
        char *line_buf = malloc((size_t)len + 1);
        long total = 0;
        ssize_t r;

        if (line_buf == NULL) {
            printf("Memory allocation error\n");
            exit(1);
        }

        while (total < len) {
            r = read(fd, line_buf + total, (size_t)(len - total));

            if (r <= 0)
                break;

            total += r;
        }

        line_buf[total] = '\0';
        printf("%s\n", line_buf);

        free(line_buf);
    }

    free(offset);
    free(length);
    close(fd);

    return 0;
}