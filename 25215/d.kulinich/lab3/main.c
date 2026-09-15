#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>



int main () {
    uid_t uid = getuid();
    uid_t euid = geteuid();
    printf("UID: %ld\n", (long)uid);
    printf("EUID: %ld\n", (long)euid);

    FILE* file = fopen("file", "r");
    if (file == NULL) {
        perror("Failed to open file for the first time");
    } else {
        printf("Successfully opened file\n");
        fclose(file);
    }
    
    int ret_c = setuid(uid); // Drop effective UID (EUID) to real UID (RUID).
    if (ret_c != 0) {
        perror("failed to set EUID");
        return 1;
    }

    uid = getuid();
    euid = geteuid();
    printf("UID: %ld\n", (long)uid);
    printf("EUID: %ld\n", (long)euid);

    file = fopen("file", "r");
    if (file == NULL) {
        perror("Failed to open file for the second time");
    } else {
        printf("Successfully opened file\n");
        fclose(file);
    }

    return 0;
}

