#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#define N 100

#include <ctype.h>

char* to_uppercase(char *array, int length) {
    for (int i = 0; i < length; i++) {
        array[i] = (char)toupper((unsigned char)array[i]);
    }
    return array;
}  

int main() {
    int descriptors[2];
    int ret = pipe(descriptors);
    if (ret == -1) {
        perror("Failed to open the pipe");
        exit(1);
    }


    pid_t pid = fork();


    if (pid == 0) {
        ret = close(descriptors[0]);
        if (ret == -1) {
            perror("Closing the unused read descriptor failed");
            exit(1);
        }


        char string[N] = "Hello World!";
        char* offset = string;
        int bytes_to_write = strnlen(string, N) + 1;
        int bytes_written = 0;
        do {
            bytes_written = write(descriptors[1], offset, bytes_to_write);
            if (bytes_written == -1) {
                perror("Write to the pipe failed");
                exit(1);
            }
            offset += bytes_written;
            bytes_to_write -= bytes_written;
        } while (bytes_to_write > 0);


        ret = close(descriptors[1]);
        if (ret == -1) {
            perror("Closing the pipe write descriptor failed");
            exit(1);
        }
    }
    else if (pid == -1) {
        perror("Fork failed");
        close(descriptors[0]);
        close(descriptors[1]);
        exit(1);
    }
    else {
        ret = close(descriptors[1]);
        if (ret == -1) {
            perror("Closing the unused write descriptor failed");
            exit(1);
        }


        char buffer[N] = {0};
        char* offset = buffer;
        int bytes_obtained = 0;
        int chunk_size = N;
        do {
            bytes_obtained = read(descriptors[0], offset, chunk_size);
            if (bytes_obtained == -1) {
                perror("Error when reading from the pipe");
                exit(1);
            }
            to_uppercase(offset, bytes_obtained);
            ret = fwrite(offset, 1, bytes_obtained, stdout);
            if (ret == -1) {
                perror("Failed to write data to stdout\n");
                exit(1);
            }
        } while (bytes_obtained != 0);

        ret = printf("\n");
        if (ret == -1) {
            perror("Failed to print newline\n");
            exit(1);
        }

        ret = close(descriptors[0]);
        if (ret == -1) {
            perror("Closing the pipe read descriptor failed");
            exit(1);
        }
    }
    exit(0);
}