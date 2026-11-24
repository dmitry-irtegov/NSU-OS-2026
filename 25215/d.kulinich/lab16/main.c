#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main(void) {
    struct termios orig_tty, new_tty;
    char response;
    ssize_t bytes_read;

    if (!isatty(STDIN_FILENO)) {
        perror("stdin not a terminal");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &orig_tty) == -1) {
        perror("tcgetattr");
        return 1;
    }

    new_tty = orig_tty;

    new_tty.c_lflag &= ~(ICANON);

    new_tty.c_cc[VMIN] = 1;
    new_tty.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_tty) == -1) {
        perror("tcsetattr");
        return 1;
    }

    bytes_read = read(STDIN_FILENO, &response, 1);

    if (bytes_read == -1) {
        perror("read");
    } else if (bytes_read == 0) {
        printf("EOF");
    } else {
        printf("\nPressed: '%c'\n", response);
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_tty) == -1) {
        perror("recovery tcsetattr");
        return 1;
    }

    return 0;
}