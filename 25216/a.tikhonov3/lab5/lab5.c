#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char c;
    int n;
    int lines = 0;
    int capacity = 16;

    long *offset = malloc(capacity * sizeof(long));
    long *length = malloc(capacity * sizeof(long));

    long start = 0;
    long pos;

    if (offset == NULL || length == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);

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
                pos = lseek(fd, 0L, SEEK_CUR) - (bytes_read - i - 1);

                if (lines == capacity) {
                    capacity *= 2;
                    offset = realloc(offset, capacity * sizeof(long));
                    length = realloc(length, capacity * sizeof(long));

                    if (offset == NULL || length == NULL) {
                        printf("Memory allocation error\n");
                        exit(1);
                    }
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
            offset = realloc(offset, capacity * sizeof(long));
            length = realloc(length, capacity * sizeof(long));
            if (offset == NULL || length == NULL) {
                    printf("Memory allocation error\n");
                    exit(1);
                }
        }

        length[lines] = pos - start;
        lines++;
    }

    printf("Line\tOffset\tLength\n");

    for (n = 0; n < lines; n++)
        printf("%d\t%ld\t%ld\n", n + 1, offset[n], length[n]);

    while (1) {
        printf("Enter line number (0 - exit): ");

        if (scanf("%d", &n) != 1 || n == 0)
            break;

        if (n < 1 || n > lines) {
            printf("No such line\n");
            continue;
        }

        lseek(fd, offset[n - 1], SEEK_SET);

        long len = length[n - 1];
        char *buf = malloc((size_t)len + 1);
        long total = 0;
        ssize_t r;

        if (buf == NULL) {
            printf("Memory allocation error\n");
            exit(1);
        }

        while (total < len) {
            r = read(fd, buf + total, (size_t)(len - total));

            if (r <= 0)
                break;

            total += r;
        }

        buf[total] = '\0';
        printf("%s\n", buf);

        free(buf);
    }

    free(offset);
    free(length);
    close(fd);

    return 0;
}