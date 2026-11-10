#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
volatile sig_atomic_t countOfBell = 0;
volatile sig_atomic_t quitflag    = 0;
void signalINTreg(int sig){
        signal(SIGINT, signalINTreg);
        (void) sig;
        write(1,"\a",1);
        countOfBell +=1;
        //signal(SIGINT, signalINTreg);

}
void signalOUTreg(int sig){
        (void) sig;
        quitflag = 1;
        signal(SIGQUIT, signalOUTreg);
    

}
int main(){
        signal(SIGINT, signalINTreg);
        signal(SIGQUIT, signalOUTreg);
        while(!quitflag){

        //todo
        }
    printf("%d",countOfBell);
    return 0;
}
