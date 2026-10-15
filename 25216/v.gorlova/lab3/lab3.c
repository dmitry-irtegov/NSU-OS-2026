#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    FILE *fin;
    uid_t ruid, euid;

    ruid = getuid();
    euid = geteuid();
    printf("Real user ID: %d\n", ruid);
    printf("Effective user ID: %d\n", euid);

    fin = fopen("HelloWord", "r");
    if (fin == NULL){
        perror("fopen");
    }
    else{
        printf("File opened successfully\n");
        fclose(fin);
    }
    
    setuid(ruid);

    printf("\nAfter setuid:\n");
    ruid = getuid();
    euid = geteuid();
    printf("Real user ID: %d\n", ruid);
    printf("Effective user ID: %d\n", euid);

    fin = fopen("HelloWord", "r");
    if (fin == NULL){
        perror("fopen");
    } 
    else{
        printf("File opened successfully\n");
        fclose(fin);
    }

    return 0;
}