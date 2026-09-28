#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

extern char *tzname[];

int main() {
    time_t now;
    struct tm *sp;
    time_t ret;
    int pret;
    ret = time(&now);
    if (ret == -1) {
        exit(1);
    }
    pret = setenv("TZ", "PST8PDT", 1);
    if (pret == -1) {
        exit(1);
    }
    sp = localtime(&now);
    if (sp == NULL) {
        exit(1);
    }
    pret = printf("%d/%d/%02d %d:%02d %s\n",
                                    sp -> tm_mon + 1, sp -> tm_mday,
                                    sp -> tm_year + 1900, sp -> tm_hour,
                                    sp -> tm_min, tzname[sp -> tm_isdst]);
    if (pret == -1) {
        exit(1);
    }
    exit(0);
}