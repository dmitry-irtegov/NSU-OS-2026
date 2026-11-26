#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

const char *editor = "nano";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    const char *path = argv[1];

    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("open(path, O_RDWR)");
        exit(1);
    }

    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0
    };

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl(F_SETLKW)");
        exit(1);
    }

    size_t cmd_size = strlen(editor) + strlen(path) + 10;
    char *cmd = malloc(cmd_size);
    if (!cmd) {
        perror("malloc");
        exit(1);
    }

    int n = snprintf(cmd, cmd_size, "%s %s", editor, path);
    if (n < 0) {
        perror("snprintf");
        exit(1);
    }

    int rc = system(cmd);
    if (rc == -1) {
        perror("system");
        exit(1);
    }

    return 0;
}
