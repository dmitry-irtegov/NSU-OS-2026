cat > lab-3.c << 'EOF'
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


#define FILENAME "data3.txt"


void print_ids_and_open() {
    FILE *f;

    printf("real UID = %ld, effective UID = %ld\n",
           (long) getuid(), (long) geteuid());

    f = fopen(FILENAME, "r");
    if(f == NULL) {
        perror(FILENAME);
    } else {
        printf("%s: opened successfully\n", FILENAME);
        fclose(f);
    }
}

int main() {
    print_ids_and_open();

    if(setuid(getuid()) == -1) {
        perror("setuid");
    }

    print_ids_and_open();
    return 0;
}
EOF