#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    time_t now;
    struct tm *sp;
    int res;

    time(&now);

    res = putenv("TZ=PST8PDT");
    if (res != 0){
        perror("putenv");
        exit(1);
    }
    tzset();

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min);
    exit(0);
}