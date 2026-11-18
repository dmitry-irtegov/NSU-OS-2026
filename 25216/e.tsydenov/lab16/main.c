#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main() {
  struct termios old_term, new_term;

  if (tcgetattr(STDIN_FILENO, &old_term) == -1) {
    perror("tcgetattr");
    return 1;
  }

  new_term = old_term;
  new_term.c_lflag &= ~ICANON;
  new_term.c_cc[VMIN] = 1;
  new_term.c_cc[VTIME] = 0;

  if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term) == -1) {
    perror("tcsetattr");
    return 1;
  }

  printf("Continue? (y/n): ");
  fflush(stdout);

  char answer;
  read(STDIN_FILENO, &answer, 1);

  if (tcsetattr(STDIN_FILENO, TCSANOW, &old_term) == -1) {
    perror("tcsetattr");
    return 1;
  }

  printf("\nYour answer: %c\n", answer);
  return 0;
}
