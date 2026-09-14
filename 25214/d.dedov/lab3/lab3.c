#include "unistd.h"
#include <stdio.h>

int main() {
    uid_t real_uid = getuid();
    uid_t eff_uid = geteuid();
    printf("real uid: %d\n", real_uid);
    printf("effective uid: %d\n", eff_uid);
    FILE* f = fopen("sample", "r");
    if (!f) {
        perror("err: fopen");
    } else {
        fclose(f);
    }

    setuid(real_uid);

    real_uid = getuid();
    eff_uid = geteuid();
    printf("real uid: %d\n", real_uid);
    printf("effective uid: %d\n", eff_uid);

    f = fopen("sample", "r");
    if (!f) {
        perror("err: fopen");
    } else {
        fclose(f);
    }
    return 0;
}
