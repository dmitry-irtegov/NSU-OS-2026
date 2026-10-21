#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINES 1000
#define MAX_LEN 1000

int main(int argc, char **argv)
{
    char c;
    int n;
    int lines = 0;

    long offset[MAX_LINES];
    long length[MAX_LINES];

    long start = 0;
    long pos;

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) 

    if (fd == -1) {
        printf("Error opening file\n");
        return 1;
    }

    offset[0] = 0;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            pos = lseek(fd, 0L, 1);

            length[lines] = pos - start - 1;
            lines++;

            if (lines < MAX_LINES)
                offset[lines] = pos;

            start = pos;
        }
    }

    pos = lseek(fd, 0L, 1);

    if (pos > start) {
        length[lines] = pos - start;
        lines++;
    }

    printf("Line\tOffset\tLength\n");

    for (n = 0; n < lines; n++) {
        printf("%d\t%ld\t%ld\n", n + 1, offset[n], length[n]);
    }

    while (1) {
        printf("Enter line number (0 - exit): ");
        scanf("%d", &n);

        if (n == 0)
            break;

        if (n < 1 || n > lines) {
            printf("No such line\n");
            continue;
        }

        lseek(fd, offset[n - 1], 0);

        {
            char buf[MAX_LEN + 1];
            long len = length[n - 1];
            long total = 0;
            int r;

            while (total < len) {
                r = read(fd, buf + total, len - total);

                if (r <= 0)
                    break;

                total += r;
            }

            buf[total] = '\0';

            printf("%s\n", buf);
        }
    }

    close(fd);

    return 0;
}