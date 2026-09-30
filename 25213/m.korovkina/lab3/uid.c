#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int open_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return errno;
    }
    printf("Managed to open the file\n");
    fclose(file);
    return 0;
}
void print_uid(uid_t* ruid, uid_t* euid) {
    *ruid = getuid();
    *euid = geteuid();
    printf("Real UID: %u\n", *ruid);
    printf("Effective UID: %u\n", *euid);
    return;
}

int main() {
    uid_t ruid, euid;
    print_uid(&ruid, &euid);
    const char* filename = "file.txt";
    int res  = open_file(filename);
    if (res != 0) {
        perror("Failed to open file");
    }
    res = setuid(ruid);
    if (res != 0) {
        perror("Failed to change euid");
        return 1;
    }
    print_uid(&ruid, &euid);
    res = open_file(filename);
    if (res != 0) {
        perror("Failed to open file");
    }
    return 0;
}
