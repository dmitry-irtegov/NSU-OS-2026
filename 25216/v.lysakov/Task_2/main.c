#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main(void)
{
    time_t now;
    struct tm *sp;
    char *time_string;

    if (putenv("TZ=PST8PDT") != 0) {
        perror("putenv");
        return 1;
    }

    tzset();

    if (time(&now) == (time_t)-1) {
        perror("time");
        return 1;
    }

    time_string = ctime(&now);
    if (time_string == NULL) {
        fprintf(stderr, "ctime failed\n");
        return 1;
    }

    printf("%s", time_string);

    sp = localtime(&now);
    if (sp == NULL) {
        fprintf(stderr, "localtime failed\n");
        return 1;
    }

    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1,
        sp->tm_mday,
        sp->tm_year,
        sp->tm_hour,
        sp->tm_min,
        tzname[sp->tm_isdst]);

    return 0;
}