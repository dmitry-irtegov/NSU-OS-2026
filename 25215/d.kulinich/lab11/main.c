#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    int eacces_found = 0;

    
    if (strchr(file, '/') != NULL) {
        execve(file, argv, envp);
        return -1; 
    }

    const char *path_env = getenv("PATH");
    if (path_env == NULL) {
        path_env = "/usr/bin:/bin";
    }

    size_t file_len = strlen(file);
    const char *p = path_env;


    while (*p != '\0') {
        const char *end = strchr(p, ':');

        if (end == NULL) {
            end = p + strlen(p);
        }

        size_t dir_len = end - p;
        
        size_t alloc_size = (dir_len == 0 ? 1 : dir_len) + 1 + file_len + 1;
        char *full_path = (char *)malloc(alloc_size);
        
        if (full_path == NULL) {
            errno = ENOMEM;
            return -1;
        }

        if (dir_len == 0) {
            strcpy(full_path, "./");
        } else {
            strncpy(full_path, p, dir_len);
            full_path[dir_len] = '/';
            full_path[dir_len + 1] = '\0';
        }
        strcat(full_path, file);

        execve(full_path, argv, envp);


        if (errno == EACCES) {
            eacces_found = 1;
        } else if (errno != ENOENT && errno != ENOTDIR) {
            int saved_errno = errno;
            free(full_path);
            errno = saved_errno;
            return -1;
        }

        free(full_path);

        if (*end == '\0') {
            break; 
        }
        p = end + 1;
    }

    if (eacces_found) {
        errno = EACCES;
    } else {
        errno = ENOENT;
    }

    return -1;
}

extern char** environ;

int main(void) {
    
    char *args[] = {
        "env",
        NULL
    };

    execvpe("env", args, environ);

    return 1;
}