#include <unistd.h>
#include <stdio.h>

int main() {
    printf("real user id: %d\neffective user id: %d\n", getuid(), geteuid());
    
    FILE* f_in = fopen("file.txt", "r");
    
    if (f_in == NULL) { 
        perror("open file ");
    } else {
        fclose(f_in);
    }
     
    if (setuid(getuid()) == -1) {
        perror("setuid failed");
        return 1; 
    }
    
    printf("real user id: %d\neffective user id: %d\n", getuid(), geteuid());
   
    f_in = fopen("file.txt", "r");
   
    if (f_in == NULL) { 
        perror("open file ");
    } else {
        fclose(f_in);
    } 

    return 0;
}