#include <stdio.h>
#include <unistd.h>

void printids(){
    printf("User UID: %lu\n", (unsigned long)getuid());
    printf("User UEID: %lu\n", (unsigned long)geteuid());
}

int openfile() {
    FILE* filep = fopen("file.txt", "r+");

    if (filep == NULL){
        perror("fopen() err");
        return -1;
    }
    printf("File opened successfully\n");

    if (fclose(filep) == -1) {
        perror("fclose() err"); 
        return -1;
    } 
    return 0;
}

int main(){

    printf("== 1 ==\n");
    printids();

    openfile();



    printf("== 2 ==\n");

    uid_t euid = geteuid();
    if (getuid() != euid) {
        
        uid_t uid = setuid(euid);
        if (uid == -1){
            perror("setuid() err");
            return -1;
        }
    }

    printids();

    openfile();

    return 0;
}