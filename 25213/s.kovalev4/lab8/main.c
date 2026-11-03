#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDWR);

    if (fd == -1) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    struct flock lock = {0};
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl(F_SETLK)");
        return EXIT_FAILURE;
    }

    size_t command_size = strlen(argv[1]) + strlen("vi -- ''") + 1;
    char command[command_size];

    int result = snprintf(command, command_size, "vi -- '%s'", argv[1]);

    if (result < 0) {
        perror("snprintf");
        return EXIT_FAILURE;
    }

    int status = system(command);

    if (status == -1) {
        perror("system");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
