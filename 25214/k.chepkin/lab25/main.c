#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
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

    pid_t pid = fork();
    if (ret == -1) {
        perror("Failed to open the pipe");
        exit(1);
    }


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
            bytes_written = write(descriptors[1], string, strnlen(string, N) + 1);
            offset += bytes_written;
            bytes_to_write -= bytes_written;
        } while (bytes_to_write > 0 && bytes_written != -1);
        if (bytes_written == -1) {
            perror("Write to the pipe failed");
            exit(1);
        }


        ret = close(descriptors[1]);
        if (ret == -1) {
            perror("Closing the pipe write descriptor failed");
            exit(1);
        }
    }
    else if (pid == -1) {
        perror("Fork failed");
        exit(1);
    }
    else {
        ret = close(descriptors[1]);
        if (ret == -1) {
            perror("Closing the unused write descriptor failed");
            exit(1);
        }


        char buffer[N];
        char* offset = buffer;
        ret = waitpid(-1, NULL, 0);
        if (ret == -1) {
            perror("waitpid failed");
            exit(1);
        }


        int bytes_obtained = 0;
        int left_to_read = N;
        do {
            bytes_obtained = read(descriptors[0], offset, left_to_read);
            offset += bytes_obtained;
            left_to_read -= bytes_obtained;
        } while (bytes_obtained != 0 && left_to_read > 0 && bytes_obtained != -1);
        if (bytes_obtained == -1) {
            perror("Error when reading from the pipe");
            exit(1);
        }


        ret = printf(
                "Message: \"%." TOSTRING(N) "s\", length of data: %lu\n",
                to_uppercase(buffer, strnlen(buffer, N)),
                strnlen(buffer, N) + 1);
        if (ret == -1) {
            perror("Failed to print message obtained from the pipe");
            exit(1);
        }


        close(descriptors[0]);
        if (ret == -1) {
            perror("Closing the pipe read descriptor failed");
            exit(1);
        }
    }
}