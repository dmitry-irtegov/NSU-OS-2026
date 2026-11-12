#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main(void)
{
    time_t now;
    time_t ret;
    struct tm *sp;

    setenv("TZ", "PST8PDT", 1);
    tzset();

    ret = time(&now);
    if (ret == (time_t)-1) {
        perror("time");
        return 1;
    }

    printf("Current local time: %s", ctime(&now));

    sp = localtime(&now);
    if (sp == NULL) {
        perror("localtime");
        return 1;
    }

    printf("In California (PDT): %d/%d/%d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);

    return 0;
}