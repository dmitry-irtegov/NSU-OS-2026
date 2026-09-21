#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define PST_OFFSET (8 * 60 * 60)

extern char *tzname[];


int main()
{
    time_t now;
    time_t cal_time;
    struct tm *sp;

    (void) time( &now );

    printf("%s", ctime( &now ) );

    /* Исходное время
    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n",
    sp->tm_mon + 1, sp->tm_mday,
    sp->tm_year, sp->tm_hour,
    sp->tm_min, tzname[sp->tm_isdst]);
    */
    
    //Время в California
    cal_time = now - PST_OFFSET;
    sp = gmtime(&cal_time);
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year, sp->tm_hour,
        sp->tm_min);

    exit(0);
}