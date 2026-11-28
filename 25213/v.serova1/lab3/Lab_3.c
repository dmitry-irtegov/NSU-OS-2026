#include <stdio.h>
#include <unistd.h>

void user_id(){
    printf("Real user ID: %lu\n", (unsigned long)getuid());
    printf("Effective user ID: %lu\n", (unsigned long)geteuid());
}

void open_file(const char* path){
    FILE* input = fopen(path, "r");
    if (input == NULL){
        perror("fopen");
    } else {
        printf("File opened\n");
        if (fclose(input) != 0) {
            perror("fclose");
        } else {
            printf("File closed\n");
        }
    }
}

int main(int argc, char *argv[]) {
    const char* path;
    if (argc < 2) {
        fprintf(stderr, "Error: no file argument given\n");
        return 1;
    } else {
       path = argv[1];
    }
    user_id();
    open_file(path);
    if (getuid() != geteuid()) {
        if (setuid(getuid()) == -1){
            perror("setuid");
        }
    }
    user_id();
    open_file(path);
    return 0;
}
