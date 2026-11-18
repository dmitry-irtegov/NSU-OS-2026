#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

int main(void)
{
    struct termios old_term, new_term;
    char answer;
    ssize_t bytes_read;

    if (tcgetattr(STDIN_FILENO, &old_term) == -1) {
        perror("tcgetattr");
        return EXIT_FAILURE;
    }

    new_term = old_term;

    new_term.c_lflag &= ~ICANON;
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) == -1) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }

    printf("Продолжить выполнение? [y/n]: ");
    fflush(stdout);

    bytes_read = read(STDIN_FILENO, &answer, 1);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) == -1) {
        perror("tcsetattr (restore)");
        return EXIT_FAILURE;
    }

    if (bytes_read != 1) {
        if (bytes_read == -1)
            perror("read");
        return EXIT_FAILURE;
    }

    if (answer == 'y' || answer == 'Y') {
        printf("\nВы нажали 'y' :)\n");
    } else if (answer == 'n' || answer == 'N') {
        printf("\nВы нажали 'n' :(\n");
    } else {
        printf("\nНеизвестный символ '%c' — ожидалось 'y' или 'n'.\n", answer);
    }

    return EXIT_SUCCESS;
}