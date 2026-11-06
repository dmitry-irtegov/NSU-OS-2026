#include <stdio.h>
#include <termios.h>
#include <unistd.h>

void set_origin(const struct termios *orig_term) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_term);
}

int main(void) {
    struct termios orig_term, new_term;
    char c;

    if (tcgetattr(STDIN_FILENO, &orig_term) == -1) {
        perror("tcgetattr");
        return 1;
    }

    new_term = orig_term;
    new_term.c_cc[VMIN] = 1;
    new_term.c_cc[VTIME] = 0;
    new_term.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Everything okay? (y/n): ");
    fflush(stdout);

    ssize_t n = read(STDIN_FILENO, &c, 1);

    if (n == -1) {
        perror("read");
        set_origin(&orig_term);
        return 1;
    } else if (n == 0) {
        fprintf(stderr, "Error: unexpected EOF\n");
        set_origin(&orig_term);
        return 1;
    }

    printf("%c\n", c);
    if (c == 'y' || c == 'Y') {
        printf("Great!\n");
    } else if (c == 'n' || c == 'N') {
        printf("Sometimes it happens...\n");
    } else {
        printf("Unexpected answer\n");
    }

    set_origin(&orig_term);

    return 0;
}
