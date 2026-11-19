#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(){
    struct termios oldt, newt;
    char result;
    if (tcgetattr(STDIN_FILENO, &oldt) == -1){
        perror("tcgetattr");
        exit(1);
    }
    newt = oldt;
    newt.c_lflag &= ~ICANON;
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1){
        perror("tcsetattr");
        exit(1);
    }
    printf("2 + 2 = ?\n");
    fflush(stdout);
    result = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (result == '4'){
        printf("\nNice work!\n");
    }
    else{
        printf("\nResult %c is incorrect. The correct one is 4.\n", result);
    }
    return 0;
}