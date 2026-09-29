#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

extern char *tzname[];

int main(void) {

    time_t now;
    struct tm *sp;

    if (setenv("TZ", "PST8PDT", 1) != 0) {
        perror("Error with setting TZ");
        return 1;
    }

    if (time(&now) == (time_t)-1) {
        perror("Error in time");
        return 1;
    }

    printf("%s", ctime( &now ) );

    return 0;
}