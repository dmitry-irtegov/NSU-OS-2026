#include <stdlib.h>
#include <stdio.h>

#include <termios.h>
#include <string.h>
#include <signal.h>

static struct termios stored_settings;

void set_keypress(void)
{
	struct termios new_settings;

	if (tcgetattr(0,&stored_settings) != 0) {
		fprintf(stderr, "Error getting terminal attributes\n");
		exit(1);
	}

	new_settings = stored_settings;

	new_settings.c_lflag &= (~ICANON & ~ECHO);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;

	if (tcsetattr(0,TCSANOW,&new_settings) != 0) {
		fprintf(stderr, "Error setting terminal attributes\n");
		exit(1);
	}
	return;
}

void reset_keypress(void)
{
	if (tcsetattr(0,TCSANOW,&stored_settings) != 0) {
		fprintf(stderr, "Error resetting terminal attributes\n");
		exit(1);
	}
	return;
}

void halndle_signal(int sig) {
	reset_keypress();
	_exit(128 + sig);
} 

int main(void)
{
	signal(SIGINT, halndle_signal);
	signal(SIGTERM, halndle_signal);
	signal(SIGHUP, halndle_signal);
	
	set_keypress();
	
	printf("Are you student? (y/n)\n");
	putchar(getchar()); 
    printf("\nok\n");
    reset_keypress();
	return 0;
}