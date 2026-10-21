#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

static struct termios old;

static void setOld(int sig) 
{
    (void)sig;
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    printf("\nCtrl+C\n");
    exit(1);
}

static int getch(void)
{
    struct termios new;

	tcgetattr(STDIN_FILENO, &old);

	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSANOW, &new);

	int c = getchar();
    if (c == 4) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old); 
        printf("\nEOF\n");
        exit(1);
    }
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	return c;
}

int main()
{
    signal(SIGINT, setOld);

    printf("Введите символ: ");
    fflush(stdout);

    int ans = getch();

    printf("\nВаш символ: %c\n", ans);

    return 0;
}