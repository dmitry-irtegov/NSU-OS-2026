#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
int countOfBell = 0;
int quitflag = 0;
void signalINTreg(int sig){
        (void) sig;
        write(1,"\a",1);
        countOfBell +=1;
        signal(SIGINT, signalINTreg);

}
void signalOUTreg(int sig){
    (void) sig;
    quitflag = 1;
    #ifdef SIGQUIT
        signal(SIGQUIT, signalOUTreg); // Для Linux
    #endif
    #ifdef SIGBREAK
        signal(SIGBREAK, signalOUTreg); // Для Windows
    #endif

}
int main(){
    signal(SIGINT, signalINTreg);
    #ifdef SIGQUIT
        signal(SIGQUIT, signalOUTreg); // Для Linux
    #endif
    #ifdef SIGBREAK
        signal(SIGBREAK, signalOUTreg); // Для Windows
    #endif
    while(!quitflag){

        //todo
        
    }
    printf("%d",countOfBell);
    return 0;
}