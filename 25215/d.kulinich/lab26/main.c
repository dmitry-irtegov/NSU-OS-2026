#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(void) {
    const char *message = "Hello, UNIX SVR4! This is a MiXeD cAsE MeSsAgE.\n";

    FILE* pipe_fp = popen("tr a-z A-Z", "w");
    
    if (pipe_fp == NULL) {
        perror("popen failed");
        return 1;
    }

    if (fputs(message, pipe_fp) == EOF) {
        perror("failed to write to tr");
        pclose(pipe_fp);
        return 1;
    }

    int status = pclose(pipe_fp);

    if (status == -1) {
        perror("pclose failed");
        return 1;
    }

    return 0;
}